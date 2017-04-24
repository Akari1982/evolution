#ifndef PERSON_H
#define PERSON_H



class Person
{
public:
    Person( const float x, const float y );
    virtual ~Person();

    void Update();
    void Draw();

    float GetRadius() const;
    float GetX() const;
    void SetX( const float x );
    float GetY() const;
    void SetY( const float y );
    float GetRotation() const;
    void SetRotation( const float rotation );

    bool IsDead() const;

    float GetKnowledge( const int know_id ) const;
    void ChangeKnowledge( const int person_id, const int know_id, const float opinion );

private:
    Person();

private:
    float m_Radius;
    float m_X;
    float m_Y;
    float m_Rotation;

    float m_Life;

    float m_DefaultRelationship;
    std::map< int, float > m_Relationship;
    std::vector< float > m_Knowledge;
};



#endif // PERSON_H
