#ifndef _INCLUDE_NOCOPYABLE_HPP_
#define _INCLUDE_NOCOPYABLE_HPP_

namespace Base {

class nocopyable {
 protected:
    nocopyable () {}

    ~nocopyable () {}
 private:
    nocopyable (const nocopyable&);

    nocopyable& operator=(const nocopyable&);
};

}
#endif
