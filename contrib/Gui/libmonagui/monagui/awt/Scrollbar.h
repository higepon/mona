/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if !defined(_SCROLLBAR_H_INCLUDED_)
#define _SCROLLBAR_H_INCLUDED_

namespace monagui {
    class Scrollbar : public Component {
    public:
        static const int HORIZONTAL = 0;
        static const int VERTICAL   = 1;

    private:
        int orientation;
        int minimum;
        int maximum;
        int blocksize;
        int preValue;
        int value;
        Event adjustmentEvent;

    public:
        Scrollbar();

        Scrollbar(int orientation);

        virtual ~Scrollbar();

        inline int getMinimum() { return this->minimum; }

        inline int getMaximum() { return this->maximum; }

        inline int getBlocksize() { return this->blocksize; }

        inline int getValue() { return this->value; }

        inline void setMinimum(int n) { this->minimum = n; }

        inline void setMaximum(int n) { this->maximum = n; }

        inline void setBlocksize(int n) { this->blocksize = n; }

        virtual void setValue(int value);

        virtual void setBounds(int x, int y, int w, int h);

        virtual void paint(Graphics* g);

        virtual void processEvent(Event* event);
    };
}

#endif /* _SCROLLBAR_H_INCLUDED_ */
