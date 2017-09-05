#ifdef  _INCLUDE_NOCOPYABLE_HPP_
#define _INCLUDE_NOCOPYABLE_HPP_

namespace HPCs{

class noncopyable {
 protected:
    noncopyable () {}

    ~noncopyable () {}
 private:
    noncopyable (const noncopyable&);

    noncopyable& operator=(const noncopyable&);
};

}
#endif
