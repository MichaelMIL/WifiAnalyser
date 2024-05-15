#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <BasePage.h>

class PageManager
{
public:
    PageManager(Arduino_GFX *gfx) : gfx(gfx) {}
    void draw();
    void update();
    void addPage(BasePage *page);
    void highlightNext();
    void highlightPrev();
    void loadPage();
    void loadPage(int page);
    void loadPage(const char *name);
    void updateLoadedPage() { pages[currentPage]->update(); }
    void loadedPageLoop()
    {
        if (pageLoaded)
            pages[currentPage]->loop();
    }
    BasePage *getLoadedPage() { return pages[currentPage]; }
    BasePage *getPage(int page) { return pages[page]; }
    BasePage *getPage(const char *name);
    void loadedPageClick() { pages[currentPage]->click(); }
    void loadedPageDoubleClick() { pages[currentPage]->doubleClick(); }
    void loadedPageLongClick() { pages[currentPage]->longClick(); }
    inline bool isPageLoaded() { return pageLoaded; }

private:
    Arduino_GFX *gfx;
    BasePage *pages[10];
    int currentPage = 0;
    int totalPages = 0;
    int menuOffset = 0;
    int menuSize = 5;
    bool pageLoaded = false;
    int menuSelected = 0;
    int oldMenuSelected = 0;
    void drawMenuSelection();
};

#endif // PAGE_MANAGER_H