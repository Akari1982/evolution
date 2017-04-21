#ifndef PERSON_H
#define PERSON_H



class Person
{
public:
    Person( const float x, const float y );
    virtual ~Person();

    void Update();
    void Draw();

    float GetTalkRadius() const;
    float GetHearRadius() const;

    float GetX() const;
    void SetX( const float x );
    float GetY() const;
    void SetY( const float y );

    bool IsDead() const;

private:
    Person();

private:
    float m_TalkRadius;
    float m_HearRadius;
    float m_X;
    float m_Y;

    float m_Life;
};



#endif // PERSON_H
