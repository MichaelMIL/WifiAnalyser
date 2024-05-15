#ifndef BASEPAGE_H
#define BASEPAGE_H

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

class BasePage
{
public:
    BasePage(Arduino_GFX *gfx, const char *name) : gfx(gfx), name(name) {}
    virtual void draw() = 0;
    virtual void update() = 0;
    virtual void init() = 0;
    virtual void exit() = 0;
    virtual void loop(){};
    virtual void click(){};
    virtual void longClick(){};
    virtual void doubleClick(){};
    virtual void setEventLoop(){};
    const char *getName() { return name; }

protected:
    Arduino_GFX *gfx;

private:
    const char *name;
};

#endif // BASEPAGE_H