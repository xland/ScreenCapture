template <typename Derived>
class Base {
public:
    void someMethod() {
        static_cast<Derived*>(this)->specificMethod(); // 基类调用派生类的方法
    }
};

class Derived : public Base<Derived> {
public:
    void specificMethod() {
        // 派生类实现特定功能
    }
};