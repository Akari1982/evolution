#ifndef PERSON_H
#define PERSON_H

#include <map>
#include <vector>



class Person
{
public:
    Person( const float x, const float y );
    virtual ~Person();

    void Update();
    void Draw( const float x, const float y );

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
    void ChangeKnowledge( const int know_id, const float value );

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
    std::vector< float > m_Confidence;
    float m_KnowTime;
};



#endif // PERSON_H
