#include "Ontogenesis.h"

#include "DebugDraw.h"
#include "Logger.h"



Ontogenesis::Ontogenesis()
{
    Generation generation;

    Gene gene;
    Condition cond;
    Expression expr;

    // Split: C_NAME( "stem" ) -> E_NAME( "n0sen" ) E_SPLIT( "n1_forw" )
    gene.name = "Split";
    cond.type = Ontogenesis::C_NAME;
    cond.name = "stem";
    gene.cond.push_back( cond );
    expr.type = Ontogenesis::E_NAME;
    expr.name = "n0_sen";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_SPLIT;
    expr.name = "n1_forw";
    gene.expr.push_back( expr );
    generation.base_genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    // ConnectLR: C_NAME( "n0_sen" ) -> E_DENDRITE( "sensor_food" ) E_AXON( "activator_left" ) E_AXON( "activator_right" )
    gene.name = "ConnectLR";
    cond.type = Ontogenesis::C_NAME;
    cond.name = "n0_sen";
    gene.cond.push_back( cond );
    expr.type = Ontogenesis::E_DENDRITE;
    expr.name = "sensor_food";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.name = "activator_left";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.name = "activator_right";
    gene.expr.push_back( expr );
    generation.base_genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    // ConnectForward: C_NAME( "n1_forw" ) -> E_DENDRITE( "sensor_food" ) E_AXON( "activator_forward" ) E_MIGRATE( "activator_forward" )
    gene.name = "ConnectForward";
    cond.type = Ontogenesis::C_NAME;
    cond.name = "n1_forw";
    gene.cond.push_back( cond );
    expr.type = Ontogenesis::E_DENDRITE;
    expr.name = "sensor_food";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.name = "activator_forward";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_MIGRATE;
    expr.name = "activator_forward";
    gene.expr.push_back( expr );
    generation.base_genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    m_Generations.push_back( generation );
}



Ontogenesis::~Ontogenesis()
{
}




void
Ontogenesis::Draw()
{
    DEBUG_DRAW.Text( 10, 10, "Total number of species: " + IntToString( m_Generations[ 0 ].species.size() ) );
}



void
Ontogenesis::LoadNetwork( Entity* entity )
{
    Logger* export_script = new Logger( "onthogenesis.txt" );



    Species species;
    species.genome = m_Generations[ 0 ].base_genome;



    Cell* cell = new Cell( "stem", 0, 0 );
    species.network.push_back( cell );

    cell = new Cell( "sensor_food_left", -5, -3 );
    cell->SetProtein( "sensor_food" );
    cell->SetProteinRadius( 8 );
    species.network.push_back( cell );

    cell = new Cell( "sensor_food_right", -5, 3 );
    cell->SetProtein( "sensor_food" );
    cell->SetProteinRadius( 8 );
    species.network.push_back( cell );

    cell = new Cell( "activator_forward", 5, 0 );
    cell->SetProtein( "activator_forward" );
    cell->SetProteinRadius( 6 );
    species.network.push_back( cell );

    cell = new Cell( "activator_left", 3, -3 );
    cell->SetProtein( "activator_left" );
    cell->SetProteinRadius( 5 );
    species.network.push_back( cell );

    cell = new Cell( "activator_right", 3, 3 );
    cell->SetProtein( "activator_right" );
    cell->SetProteinRadius( 5 );
    species.network.push_back( cell );

    export_script->Log( "Step: 0\n" );
    export_script->Log( "    add stem sensor_food sensor_food activator_forward activator_left activator_right\n" );



    bool changes = true;
    int cycles = 0;
    while( changes == true && cycles < 100 )
    {
        ++cycles;
        changes = false;

        export_script->Log( "Step: " + IntToString( cycles ) + "\n" );

        std::vector< std::vector< unsigned int > > species_genes;

        for( size_t i = 0; i < species.network.size(); ++i )
        {
            std::vector< unsigned int > expr_genes;

            export_script->Log( "    cell_" + IntToString( i ) + ": " + species.network[ i ]->GetTypeName() + "\n" );

            for( size_t gene_id = 0; gene_id < species.genome.size(); ++gene_id )
            {
                Gene gene = species.genome[ gene_id ];
                bool exec = true;

                export_script->Log( "        gene " + gene.name + "\n" );

                for( size_t cond_id = 0; cond_id < gene.cond.size(); ++cond_id )
                {
                    Condition cond = gene.cond[ cond_id ];

                    switch( cond.type )
                    {
                        case Ontogenesis::C_NAME:
                        {
                            if( species.network[ i ]->GetTypeName() != cond.name )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_NNAME:
                        {
                            if( species.network[ i ]->GetTypeName() == cond.name )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_PROTEIN:
                        {
                            std::vector< PowerProtein > powers;
                            bool power = SearchProtein( cond.name, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            if( power == false )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_NPROTEIN:
                        {
                            std::vector< PowerProtein > powers;
                            bool power = SearchProtein( cond.name, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            if( power == true )
                            {
                                exec = false;
                            }
                        }
                        break;
                    }
                }

                if( exec == true )
                {
                    export_script->Log( "            activated\n" );
                    expr_genes.push_back( gene_id );
                }
            }

            species_genes.push_back( expr_genes );
        }

        export_script->Log( "\nexpress\n" );

        size_t net_size = species.network.size();
        for( size_t i = 0; i < net_size; ++i )
        {
            export_script->Log( "    cell_" + IntToString( i ) + ": " + species.network[ i ]->GetTypeName() + "\n" );

            for( size_t ge_id = 0; ge_id < species_genes[ i ].size(); ++ge_id )
            {
                size_t gene_id = species_genes[ i ][ ge_id ];

                for( size_t expr_id = 0; expr_id < species.genome[ gene_id ].expr.size(); ++expr_id )
                {
                    Expression expr = species.genome[ gene_id ].expr[ expr_id ];

                    switch( expr.type )
                    {
                        case Ontogenesis::E_NAME:
                        {
                            if( species.network[ i ]->GetTypeName() != expr.name )
                            {
                                species.network[ i ]->SetTypeName( expr.name );
                                changes = true;
                                export_script->Log( "        E_NAME(\"" + expr.name + "\") expressed.\n" );
                            }
                            else
                            {
                                export_script->Log( "        E_NAME(\"" + expr.name + "\") depressed.\n" );
                            }
                        }
                        break;
                        case Ontogenesis::E_SPLIT:
                        {
                            int x = 0;
                            int y = 0;
                            bool place = FindPlaceForCell( species.network[ i ]->GetX(), species.network[ i ]->GetY(), 1, x, y );
                            if( place == true )
                            {
                                Cell* cell = new Cell( expr.name, x, y );
                                species.network.push_back( cell );
                                changes = true;
                                export_script->Log( "        E_SPLIT(\"" + expr.name + "\") expressed.\n" );
                            }
                            else
                            {
                                export_script->Log( "        E_SPLIT(\"" + expr.name + "\") no place.\n" );
                            }
                        }
                        break;
                        case Ontogenesis::E_MIGRATE:
                        {
                            export_script->Log( "        E_MIGRATE(\"" + expr.name + "\") expressed.\n" );
                            std::vector< PowerProtein > powers;
                            SearchProtein( expr.name, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                Cell* cell = species.network[ powers[ c ].cell_id ];
                                int x = 0;
                                int y = 0;
                                bool place = FindPlaceForCell( cell->GetX(), cell->GetY(), cell->GetProteinRadius(), x, y );
                                if( place == true )
                                {
                                    species.network[ i ]->SetX( x );
                                    species.network[ i ]->SetY( y );
                                }
                            }
                            powers.clear();
                        }
                        break;
                        case Ontogenesis::E_PROTEIN:
                        {
                            if( species.network[ i ]->GetProtein() != expr.name )
                            {
                                species.network[ i ]->SetProtein( expr.name );
                                changes = true;
                                export_script->Log( "        E_PROTEIN(\"" + expr.name + "\") expressed.\n" );
                            }
                            else
                            {
                                export_script->Log( "        E_PROTEIN(\"" + expr.name + "\") depressed.\n" );
                            }
                        }
                        break;
                        case Ontogenesis::E_DENDRITE:
                        {
                            export_script->Log( "        E_DENDRITE(\"" + expr.name + "\") expressed.\n" );
                            std::vector< PowerProtein > powers;
                            SearchProtein( expr.name, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                species.network[ i ]->AddSynapse( powers[ c ].power, false, species.network[ powers[ c ].cell_id ] );
                            }
                            powers.clear();
                        }
                        break;
                        case Ontogenesis::E_AXON:
                        {
                            export_script->Log( "        E_AXON(\"" + expr.name + "\") expressed.\n" );
                            std::vector< PowerProtein > powers;
                            SearchProtein( expr.name, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                species.network[ powers[ c ].cell_id ]->AddSynapse( powers[ c ].power, false, species.network[ i ] );
                            }
                            powers.clear();
                        }
                        break;
                    }
                }
            }
        }

        species_genes.clear();
    }



    entity->AddNetwork( species.network );



    m_Generations[ 0 ].species.push_back( species );
}



bool
Ontogenesis::SearchProtein( const Ogre::String name, const int x, const int y, std::vector< PowerProtein >& powers )
{
    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        if( m_Network[ i ].protein == name )
        {
            int x2 = m_Network[ i ].x;
            int y2 = m_Network[ i ].y;
            float distance = sqrt( ( x2 - x ) * ( x2 - x ) + ( y2 - y ) * ( y2 - y ) );
            if( distance < m_Network[ i ].protein_radius )
            {
                PowerProtein pp;
                pp.power = 1.0;
                pp.cell_id = i;
                powers.push_back( pp );
            }
        }
    }

    if( powers.size() > 0 )
    {
        return true;
    }
    return false;
}



bool
Ontogenesis::FindPlaceForCell( const int x, const int y, const int radius, int &new_x, int &new_y )
{
    for( int r = 1; r <= radius; ++r )
    {
        new_y = y - r;
        for( int i = -r + 1; i <= r; ++i )
        {
            new_x = x + i;
            if( IsCell( new_x, new_y ) == false )
            {
                return true;
            }
        }
        for( int i = -r + 1; i <= r; ++i )
        {
            new_y = y + i;
            if( IsCell( new_x, new_y ) == false )
            {
                return true;
            }
        }
        for( int i = r - 1; i <= -r; --i )
        {
            new_x = x + i;
            if( IsCell( new_x, new_y ) == false )
            {
                return true;
            }
        }
        for( int i = r - 1; i <= -r; --i )
        {
            new_y = y + i;
            if( IsCell( new_x, new_y ) == false )
            {
                return true;
            }
        }
    }

    return false;
}



bool
Ontogenesis::IsCell( const int x, const int y )
{
    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        if( m_Network[ i ].x == x && m_Network[ i ].y == y )
        {
            return true;
        }
    }

    return false;
}