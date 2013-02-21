#ifndef HAVE_PRIQUE_H
#define HAVE_PRIQUE_H

#include <stddef.h>
#include <vector>

template <class C>
bool default_lt (const C& a, const C& b) { return a < b; }

template <class C>
bool default_gt (const C& a, const C& b) { return a > b; }

template <class C, bool (* lt ) (const C&, const C&) = default_lt<C> >
struct Prique {
    void push (C x);
    void just_pop ();
    C pop ();
    C top () { return elems.at(0); }
    bool empty () { return elems.empty(); }
    size_t size () { return elems.size(); }

  private:
    std::vector<C> elems;

    void bubble_up (size_t i);
    void bubble_down (size_t i);
};

template <class C, bool (* lt ) (const C&, const C&)>
void Prique<C, lt>::push (C x) {
    size_t last = elems.size();
    elems.push_back(x);
    bubble_up(last);
}
template <class C, bool (* lt ) (const C&, const C&)>
void Prique<C, lt>::just_pop () {
    elems.at(0) = elems.back();
    elems.pop_back();
    if (!empty()) bubble_down(0);
}
template <class C, bool (* lt ) (const C&, const C&)>
C Prique<C, lt>::pop () {
    C r = elems.front();
    just_pop();
    return r;
}

template <class C, bool (* lt ) (const C&, const C&)>
void Prique<C, lt>::bubble_up (size_t i) {
    if (i == 0) return;
    size_t parent = (i - 1) / 2;
    if (lt(elems[parent], elems[i])) return;
    std::swap(elems[parent], elems[i]);
    bubble_up(parent);
}

template <class C, bool (* lt ) (const C&, const C&)>
void Prique<C, lt>::bubble_down (size_t i) {
    size_t c1 = i * 2 + 1;
    size_t c2 = c1 + 1;
    if (c1 >= elems.size()) return;
    if (c2 >= elems.size()) {
        if (lt(elems[i], elems[c1])) return;
        std::swap(elems[i], elems[c1]);
        bubble_down(c1);
    }
    if (lt(elems[i], elems[c1]) && lt(elems[i], elems[c2]))
        return;
    if (lt(elems[c1], elems[c2])) {
        std::swap(elems[i], elems[c1]);
        bubble_down(c1);
    }
    else {
        std::swap(elems[i], elems[c2]);
        bubble_down(c2);
    }
}


#endif
