#include "Ontogenesis.h"

#include "DebugDraw.h"
#include "Logger.h"



Ontogenesis::Ontogenesis()
{
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
    m_Genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    // ConnectLR: C_NAME( "n0_sen" ) -> E_DENDRITE( "sensor" ) E_AXON( "left" ) E_AXON( "right" )
    gene.name = "ConnectLR";
    cond.type = Ontogenesis::C_NAME;
    cond.name = "n0_sen";
    gene.cond.push_back( cond );
    expr.type = Ontogenesis::E_DENDRITE;
    expr.name = "sensor";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.name = "left";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.name = "right";
    gene.expr.push_back( expr );
    m_Genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    // ConnectForward: C_NAME( "n1_forw" ) -> E_DENDRITE( "sensor" ) E_AXON( "forward" ) E_MIGRATE( "forward" )
    gene.name = "ConnectForward";
    cond.type = Ontogenesis::C_NAME;
    cond.name = "n1_forw";
    gene.cond.push_back( cond );
    expr.type = Ontogenesis::E_DENDRITE;
    expr.name = "sensor";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.name = "forward";
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_MIGRATE;
    expr.name = "forward";
    gene.expr.push_back( expr );
    m_Genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();
}



Ontogenesis::~Ontogenesis()
{
}



void
Ontogenesis::LoadNetwork( Entity* entity )
{
    Logger* export_script = new Logger( "onthogenesis.txt" );

    Cell cell;
    cell.name = "stem";
    cell.x = 0;
    cell.y = 0;
    m_Network.push_back( cell );

    cell.name = "sensor";
    cell.x = -5;
    cell.y = -3;
    cell.protein = "sensor";
    cell.protein_radius = 8;
    m_Network.push_back( cell );

    cell.name = "sensor";
    cell.x = -5;
    cell.y = 3;
    cell.protein = "sensor";
    cell.protein_radius = 8;
    m_Network.push_back( cell );

    cell.name = "forward";
    cell.x = 5;
    cell.y = 0;
    cell.protein = "forward";
    cell.protein_radius = 6;
    m_Network.push_back( cell );

    cell.name = "left";
    cell.x = 3;
    cell.y = -3;
    cell.protein = "left";
    cell.protein_radius = 5;
    m_Network.push_back( cell );

    cell.name = "right";
    cell.x = 3;
    cell.y = 3;
    cell.protein = "right";
    cell.protein_radius = 5;
    m_Network.push_back( cell );

    export_script->Log( "Step: 0\n" );
    export_script->Log( "    add stem sensor sensor forward left right\n" );



    bool changes = true;
    int cycles = 0;
    while( changes == true && cycles < 100 )
    {
        ++cycles;
        changes = false;

        export_script->Log( "Step: " + IntToString( cycles ) + "\n" );

        for( size_t i = 0; i < m_Network.size(); ++i )
        {
            export_script->Log( "    cell_" + IntToString( i ) + ": " + m_Network[ i ].name + "\n" );

            for( size_t gene_id = 0; gene_id < m_Genome.size(); ++gene_id )
            {
                Gene gene = m_Genome[ gene_id ];
                bool exec = true;

                export_script->Log( "        gene " + gene.name + "\n" );

                for( size_t cond_id = 0; cond_id < gene.cond.size(); ++cond_id )
                {
                    Condition cond = gene.cond[ cond_id ];

                    switch( cond.type )
                    {
                        case Ontogenesis::C_NAME:
                        {
                            if( m_Network[ i ].name != cond.name )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_NNAME:
                        {
                            if( m_Network[ i ].name == cond.name )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_PROTEIN:
                        {
                            std::vector< PowerProtein > powers;
                            bool power = SearchProtein( cond.name, m_Network[ i ].x, m_Network[ i ].y, powers );
                            if( power == false )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_NPROTEIN:
                        {
                            std::vector< PowerProtein > powers;
                            bool power = SearchProtein( cond.name, m_Network[ i ].x, m_Network[ i ].y, powers );
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
                    m_Network[ i ].expr_genes.push_back( gene_id );
                }
            }
        }

        export_script->Log( "\nexpress\n" );

        size_t net = m_Network.size();
        for( size_t i = 0; i < net; ++i )
        {
            export_script->Log( "    cell_" + IntToString( i ) + ": " + m_Network[ i ].name + "\n" );

            for( size_t ge_id = 0; ge_id < m_Network[ i ].expr_genes.size(); ++ge_id )
            {
                size_t gene_id = m_Network[ i ].expr_genes[ ge_id ];

                for( size_t expr_id = 0; expr_id < m_Genome[ gene_id ].expr.size(); ++expr_id )
                {
                    Expression expr = m_Genome[ gene_id ].expr[ expr_id ];

                    switch( expr.type )
                    {
                        case Ontogenesis::E_NAME:
                        {
                            if( m_Network[ i ].name != expr.name )
                            {
                                m_Network[ i ].name = expr.name;
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
                            bool place = FindPlaceForCell( m_Network[ i ].x, m_Network[ i ].y, 1, x, y );
                            if( place == true )
                            {
                                Cell new_cell;
                                new_cell.name = expr.name;
                                new_cell.x = x;
                                new_cell.y = y;
                                m_Network.push_back( new_cell );
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
                            SearchProtein( expr.name, m_Network[ i ].x, m_Network[ i ].y, powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                Cell cell = m_Network[ powers[ c ].cell_id ];
                                int x = 0;
                                int y = 0;
                                bool place = FindPlaceForCell( cell.x, cell.y, cell.protein_radius, x, y );
                                if( place == true )
                                {
                                    m_Network[ i ].x = x;
                                    m_Network[ i ].y = y;
                                }
                            }
                            powers.clear();
                        }
                        break;
                        case Ontogenesis::E_PROTEIN:
                        {
                            if( m_Network[ i ].protein != expr.name )
                            {
                                m_Network[ i ].protein = expr.name;
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
                            SearchProtein( expr.name, m_Network[ i ].x, m_Network[ i ].y, powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                Synapse synapse;
                                synapse.power = powers[ c ].power;
                                synapse.cell_id = powers[ c ].cell_id;
                                m_Network[ i ].synapses.push_back( synapse );
                            }
                            powers.clear();
                        }
                        break;
                        case Ontogenesis::E_AXON:
                        {
                            export_script->Log( "        E_AXON(\"" + expr.name + "\") expressed.\n" );
                            std::vector< PowerProtein > powers;
                            SearchProtein( expr.name, m_Network[ i ].x, m_Network[ i ].y, powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                export_script->Log( "        1\n" );
                                Synapse synapse;
                                synapse.power = powers[ c ].power;
                                synapse.cell_id = i;
                                m_Network[ powers[ c ].cell_id ].synapses.push_back( synapse );
                            }
                            powers.clear();
                        }
                        break;
                    }
                }
            }

            m_Network[ i ].expr_genes.clear();
        }
    }



    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        entity->AddNeuron( m_Network[ i ].x, m_Network[ i ].y, 0.0f, 0.0f );
    }



/*
    unsigned int self_id = 0;
    node = m_File.RootElement()->FirstChild();
    while( node != NULL )
    {
        if( node->Type() == TiXmlNode::TINYXML_ELEMENT && node->ValueStr() == "neuron" )
        {
            TiXmlNode* node2 = node->FirstChild();
            while( node2 != NULL )
            {
                if( node2->Type() == TiXmlNode::TINYXML_ELEMENT && node2->ValueStr() == "synapse" )
                {
                    entity->AddSynapse( self_id, GetString( node2, "type" ), GetFloat( node2, "power" ), GetBool( node2, "inverted", false ), GetInt( node2, "neuron_id" ), GetFloat( node2, "length" ), GetFloat( node2, "degree" ) );
                }
                node2 = node2->NextSibling();
            }
            ++self_id;
        }
        node = node->NextSibling();
    }
*/



    //m_Network.clear();
}



void
Ontogenesis::Update()
{
    float global_x = 600.0f;
    float global_y = 350.0f;
    float scale = 20.0f;

    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        Cell cell = m_Network[ i ];
        float x = global_x + cell.x * scale;
        float y = global_y + cell.y * scale;
        float radius = 2.0f;
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
        DEBUG_DRAW.Circle( x, y, cell.protein_radius * scale );
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
        DEBUG_DRAW.Quad( x - radius, y - radius, x + radius, y - radius, x + radius, y + radius, x - radius, y + radius );
        for( size_t j = 0; j < cell.synapses.size(); ++j )
        {
            Cell cell_con = m_Network[ cell.synapses[ j ].cell_id ];
            float x2 = global_x + cell_con.x * scale;
            float y2 = global_y + cell_con.y * scale;
            DEBUG_DRAW.Line( x, y, x2, y2 );
        }
    }
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