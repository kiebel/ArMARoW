#include <iostream>

using namespace std;

struct A
{
    int a[2048];
    A(){}
};

struct B
{
    int a[2048];
    A& down()
    {
        a[1]+=1;
        return reinterpret_cast<A&>(*this);
    }
    static B& up(A& a)
    {
        a.a[1]-=1;
        return reinterpret_cast<B&>(a);
    }
};

struct LA
{
    void send(A& a)
    {
        cout << "LA:send ref - " << a.a[1] << endl;
    }

    void recv(A& a)
    {
        a.a[1]=1;
        cout << "LA::recv ref - " << a.a[1] << endl;
    }
};

struct LB : public LA
{
    void send(B& b)
    {
        cout << "LB:send ref - " << b.a[1] << endl;
        LA::send(b.down());
    }

    void recv(B& b)
    {
        A& a=b.down();
        LA::recv(a);
        b=B::up(a);
        cout << "LB::recv ref - " << b.a[1] << endl;
    }
};

int main()
{
    B b;
    b.a[1]=0;
    LB l;
    l.send(b);
    l.recv(b);
    return 0;
}
