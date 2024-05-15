#include <PageManager.h>
#include <BasePage.h>

void PageManager::draw()
{
    gfx->fillScreen(0);
    pageLoaded = false;
    pages[currentPage]->exit();
    drawMenuSelection();
}

void PageManager::update()
{
    drawMenuSelection();
}

void PageManager::addPage(BasePage *page)
{
    if (totalPages < 10)
    {
        pages[totalPages] = page;
        totalPages++;
    }
}

void PageManager::highlightNext()
{
    oldMenuSelected = menuSelected;
    if (menuSelected < totalPages - 1)
    {
        menuSelected++;
        drawMenuSelection();
    }
    else
    {
        menuSelected = 0;
        drawMenuSelection();
    }
}

void PageManager::highlightPrev()
{
    oldMenuSelected = menuSelected;

    if (menuSelected > 0)
    {
        menuSelected--;
        drawMenuSelection();
    }
    else
    {
        menuSelected = totalPages - 1;
        drawMenuSelection();
    }
}

void PageManager::loadPage()
{
    currentPage = menuSelected;
    pageLoaded = true;
    Serial.println("init page");
    pages[currentPage]->init();
    Serial.println("draw page");
    pages[currentPage]->draw();
    Serial.println("page loaded");
}
BasePage *PageManager::getPage(const char *name)
{
    for (int i = 0; i < totalPages; i++)
    {
        if (strcmp(name, pages[i]->getName()) == 0)
        {
            return pages[i];
        }
    }
    return nullptr;
}
void PageManager::loadPage(int page)
{
    if (page < totalPages)
    {
        currentPage = page;
        pageLoaded = true;
        pages[currentPage]->init();
        pages[currentPage]->draw();
    }
}

void PageManager::loadPage(const char *name)
{
    for (int i = 0; i < totalPages; i++)
    {
        if (strcmp(name, pages[i]->getName()) == 0)
        {
            loadPage(i);
            return;
        }
    }
}

void PageManager::drawMenuSelection()
{
    // Draw menu
    if (pageLoaded)
    {
        pageLoaded = false;
        draw();
        return;
    }
    // add title "Menu"
    gfx->setTextSize(2);
    gfx->setTextColor(0xFFFF);
    gfx->setCursor(0, 0);
    gfx->print("Menu");

    // add menu items with selection box for selected item and index number
    gfx->setTextSize(1);
    for (int i = 0; i < menuSize; i++)
    {
        if (menuOffset + i < totalPages)
        {
            if (menuOffset + i == menuSelected)
            {
                // fill old selection box with black
                gfx->fillRect(0, 20 + oldMenuSelected * 10, 10, 10, 0x0000);
                gfx->fillRect(0, 20 + i * 10, 10, 10, 0xFFFF);
            }
            gfx->setCursor(20, 20 + i * 10);
            gfx->print(i + menuOffset);
            gfx->print(". ");
            gfx->print(pages[menuOffset + i]->getName());
        }
    }
}
