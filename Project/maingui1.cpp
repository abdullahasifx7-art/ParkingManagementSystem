// ================================================================
//  main_gui.cpp
//  Grand Valet Parking — SFML GUI
//  Luxury navy + gold theme
//  Connects directly to all backend classes
// ================================================================

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>

#include "ParkingLot.h"
#include "ParkingSlot.h"
#include "FileHandler.h"
#include "Admin.h"
#include "Car.h"
#include "Bike.h"
#include "Truck.h"
#include "Bill.h"
#include "Vehicle.h"

using namespace std;

// ================================================================
//  COLOR PALETTE  — luxury navy + gold
// ================================================================
const sf::Color C_BG(8, 14, 30);
const sf::Color C_PANEL(12, 20, 44);
const sf::Color C_SIDEBAR(6, 10, 22);
const sf::Color C_GOLD(201, 161, 76);
const sf::Color C_GOLD_DARK(140, 105, 40);
const sf::Color C_GOLD_LIGHT(230, 195, 120);
const sf::Color C_WHITE(240, 238, 230);
const sf::Color C_MUTED(130, 125, 110);
const sf::Color C_SUCCESS(60, 180, 100);
const sf::Color C_ERROR(220, 70, 70);
const sf::Color C_SLOT_FREE(40, 160, 80);
const sf::Color C_SLOT_BUSY(190, 50, 50);
const sf::Color C_SLOT_MAINT(180, 140, 30);
const sf::Color C_INPUT_BG(18, 26, 55);
const sf::Color C_INPUT_ACT(25, 38, 80);
const sf::Color C_DIVIDER(40, 50, 80);

enum class Screen {
    WELCOME,
    ROLE_SELECT,
    ADMIN_LOGIN,
    ADMIN_DASHBOARD,
    USER_DASHBOARD,
    PARK_VEHICLE,
    EXIT_VEHICLE,
    SLOT_GRID,
    RATE_MANAGEMENT,
    VIEW_HISTORY,
    VIEW_PARKED,
    MANAGE_SLOTS,
    FIND_VEHICLE,
    GENERATE_REPORT,
    SLOT_MAINTENANCE
};

// ================================================================
//  HELPER STRUCTS
// ================================================================
struct InputField {
    sf::FloatRect bounds;
    string value;
    string placeholder;
    bool active = false;
    bool isPassword = false;
};

struct Button {
    sf::FloatRect bounds;
    string label;
    sf::Color fill;
    sf::Color textColor;
    bool hovered = false;
};

// ================================================================
//  UTILITY FUNCTIONS
// ================================================================
string timeToStr(time_t t) {
    struct tm buf;
#ifdef _WIN32
    localtime_s(&buf, &t);
#else
    struct tm* p = localtime(&t); if (p) buf = *p;
#endif
    char out[20];
    snprintf(out, sizeof(out), "%02d:%02d:%02d", buf.tm_hour, buf.tm_min, buf.tm_sec);
    return string(out);
}

string dateTimeToStr(time_t t) {
    struct tm buf;
#ifdef _WIN32
    localtime_s(&buf, &t);
#else
    struct tm* p = localtime(&t); if (p) buf = *p;
#endif
    char out[32];
    snprintf(out, sizeof(out), "%04d-%02d-%02d %02d:%02d",
        buf.tm_year + 1900, buf.tm_mon + 1, buf.tm_mday, buf.tm_hour, buf.tm_min);
    return string(out);
}

ParkingSlot* findSlotByEntryId(ParkingLot* lot, const string& eid) {
    for (int i = 0; i < lot->getTotalSlots(); i++) {
        ParkingSlot* s = lot->getSlot(i);
        if (!s) continue;
        Vehicle* v = s->getParkedVehicle();
        if (v && v->getEntryId() == eid) return s;
    }
    return nullptr;
}

// ================================================================
//  DRAWING HELPERS
// ================================================================
void drawRoundRect(sf::RenderWindow& w, float x, float y, float width, float height,
    float r, sf::Color fill, sf::Color outline = sf::Color::Transparent, float outlineThick = 0) {
    sf::RectangleShape rect;
    rect.setSize({ width, height - 2 * r });
    rect.setPosition(x, y + r);
    rect.setFillColor(fill);
    if (outlineThick > 0) { rect.setOutlineColor(outline); rect.setOutlineThickness(0); }
    w.draw(rect);
    rect.setSize({ width - 2 * r, height });
    rect.setPosition(x + r, y);
    w.draw(rect);
    sf::CircleShape c(r);
    c.setFillColor(fill);
    float cx[4] = { x, x + width - 2 * r, x, x + width - 2 * r };
    float cy[4] = { y, y, y + height - 2 * r, y + height - 2 * r };
    for (int i = 0; i < 4; i++) { c.setPosition(cx[i], cy[i]); w.draw(c); }
    if (outlineThick > 0) {
        sf::RectangleShape ol;
        ol.setSize({ width, height });
        ol.setPosition(x, y);
        ol.setFillColor(sf::Color::Transparent);
        ol.setOutlineColor(outline);
        ol.setOutlineThickness(outlineThick);
        w.draw(ol);
    }
}

void drawText(sf::RenderWindow& w, sf::Font& f, const string& s,
    float x, float y, unsigned int size, sf::Color col,
    bool bold = false, bool center = false) {
    sf::Text t;
    t.setFont(f);
    t.setString(s);
    t.setCharacterSize(size);
    t.setFillColor(col);
    if (bold) t.setStyle(sf::Text::Bold);
    if (center) {
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2, b.top + b.height / 2);
    }
    t.setPosition(x, y);
    w.draw(t);
}

void drawDivider(sf::RenderWindow& w, float x, float y, float width, float thick = 1.f) {
    sf::RectangleShape line({ width, thick });
    line.setPosition(x, y);
    line.setFillColor(C_GOLD_DARK);
    w.draw(line);
}

bool drawButton(sf::RenderWindow& w, sf::Font& f, Button& btn,
    const sf::Event& ev, bool clickThisFrame) {
    bool clicked = false;
    sf::Vector2f mouse;
    if (ev.type == sf::Event::MouseMoved)
        mouse = { (float)ev.mouseMove.x, (float)ev.mouseMove.y };
    else if (ev.type == sf::Event::MouseButtonPressed)
        mouse = { (float)ev.mouseButton.x, (float)ev.mouseButton.y };
    btn.hovered = btn.bounds.contains(mouse);
    sf::Color fill = btn.fill;
    if (btn.hovered) fill = sf::Color(
        min(255, (int)fill.r + 30),
        min(255, (int)fill.g + 20),
        min(255, (int)fill.b + 10));
    drawRoundRect(w, btn.bounds.left, btn.bounds.top,
        btn.bounds.width, btn.bounds.height, 6.f, fill, C_GOLD, 1.f);
    sf::Text t;
    t.setFont(f); t.setString(btn.label); t.setCharacterSize(15);
    t.setFillColor(btn.textColor); t.setStyle(sf::Text::Bold);
    sf::FloatRect tb = t.getLocalBounds();
    t.setPosition(btn.bounds.left + (btn.bounds.width - tb.width) / 2 - tb.left,
        btn.bounds.top + (btn.bounds.height - tb.height) / 2 - tb.top);
    w.draw(t);
    if (clickThisFrame && btn.hovered) clicked = true;
    return clicked;
}

void drawInput(sf::RenderWindow& w, sf::Font& f, InputField& inp) {
    sf::Color bg = inp.active ? C_INPUT_ACT : C_INPUT_BG;
    sf::Color border = inp.active ? C_GOLD : C_DIVIDER;
    drawRoundRect(w, inp.bounds.left, inp.bounds.top,
        inp.bounds.width, inp.bounds.height, 5.f, bg, border, inp.active ? 1.5f : 1.f);
    string display = inp.value.empty() ? inp.placeholder : inp.value;
    if (inp.isPassword && !inp.value.empty()) display = string(inp.value.size(), '*');
    sf::Color textCol = inp.value.empty() ? C_MUTED : C_WHITE;
    sf::Text t;
    t.setFont(f);
    t.setString(display + (inp.active ? "|" : ""));
    t.setCharacterSize(14); t.setFillColor(textCol);
    t.setPosition(inp.bounds.left + 12, inp.bounds.top + inp.bounds.height / 2 - 9);
    w.draw(t);
}

bool drawNavBtn(sf::RenderWindow& w, sf::Font& f, float x, float y, float width,
    const string& label, bool active,
    const sf::Event& ev, bool clickThisFrame) {
    sf::FloatRect b(x, y, width, 44);
    sf::Vector2f mouse;
    if (ev.type == sf::Event::MouseMoved) mouse = { (float)ev.mouseMove.x, (float)ev.mouseMove.y };
    else if (ev.type == sf::Event::MouseButtonPressed) mouse = { (float)ev.mouseButton.x, (float)ev.mouseButton.y };
    bool hov = b.contains(mouse);
    sf::Color fill = active ? sf::Color(40, 55, 100) : (hov ? sf::Color(20, 30, 60) : sf::Color::Transparent);
    sf::RectangleShape r({ width, 44 });
    r.setPosition(x, y); r.setFillColor(fill); w.draw(r);
    if (active) {
        sf::RectangleShape bar({ 3.f, 44.f });
        bar.setPosition(x, y); bar.setFillColor(C_GOLD); w.draw(bar);
    }
    sf::Text t; t.setFont(f); t.setString(label); t.setCharacterSize(13);
    t.setFillColor(active ? C_GOLD : (hov ? C_WHITE : C_MUTED));
    if (active) t.setStyle(sf::Text::Bold);
    t.setPosition(x + 18, y + 13); w.draw(t);
    return (clickThisFrame && hov);
}

void drawStatCard(sf::RenderWindow& w, sf::Font& f, float x, float y,
    float width, float height, const string& label, const string& value) {
    drawRoundRect(w, x, y, width, height, 8.f, C_PANEL, C_GOLD_DARK, 1.f);
    drawDivider(w, x + 16, y + height - 4, width - 32, 2.f);
    sf::RectangleShape top({ width, 3.f });
    top.setPosition(x, y); top.setFillColor(C_GOLD); w.draw(top);
    drawText(w, f, value, x + width / 2, y + height / 2 - 10, 28, C_GOLD, true, true);
    drawText(w, f, label, x + width / 2, y + height - 22, 11, C_MUTED, false, true);
}

void drawSectionHeading(sf::RenderWindow& w, sf::Font& f,
    const string& title, float x, float y, float width) {
    drawText(w, f, title, x, y, 16, C_GOLD, true);
    drawDivider(w, x, y + 24, width);
}

void drawHeader(sf::RenderWindow& w, sf::Font& f,
    const string& title, float contentX, float contentW) {
    sf::RectangleShape hdr({ contentW, 58.f });
    hdr.setPosition(contentX, 0); hdr.setFillColor(sf::Color(10, 16, 36)); w.draw(hdr);
    drawDivider(w, contentX, 57, contentW, 1.5f);
    sf::RectangleShape acc({ 3.f, 30.f });
    acc.setPosition(contentX + 20, 14); acc.setFillColor(C_GOLD); w.draw(acc);
    drawText(w, f, title, contentX + 32, 18, 18, C_WHITE, true);
}

int drawTypeSelector(sf::RenderWindow& w, sf::Font& f, float x, float y,
    int selected, const sf::Event& ev, bool clickThisFrame) {
    string labels[3] = { "Car", "Bike", "Truck" };
    float bw = 110, bh = 38, gap = 10;
    for (int i = 0; i < 3; i++) {
        float bx = x + i * (bw + gap);
        sf::FloatRect b(bx, y, bw, bh);
        sf::Vector2f mouse;
        if (ev.type == sf::Event::MouseMoved) mouse = { (float)ev.mouseMove.x, (float)ev.mouseMove.y };
        else if (ev.type == sf::Event::MouseButtonPressed) mouse = { (float)ev.mouseButton.x, (float)ev.mouseButton.y };
        bool hov = b.contains(mouse), sel = (selected == i);
        sf::Color fill = sel ? C_GOLD_DARK : (hov ? sf::Color(25, 35, 70) : C_INPUT_BG);
        sf::Color border = sel ? C_GOLD : C_DIVIDER;
        drawRoundRect(w, bx, y, bw, bh, 6.f, fill, border, sel ? 1.5f : 1.f);
        drawText(w, f, labels[i], bx + bw / 2, y + bh / 2 - 7, 13,
            sel ? C_GOLD : C_MUTED, sel, true);
        if (clickThisFrame && hov) return i;
    }
    return selected;
}

// ================================================================
//  MAIN APPLICATION CLASS
// ================================================================
class GrandValetApp {
public:
    sf::RenderWindow window;
    sf::Font         font;
    sf::Texture      texBg, texLogo, texCar, texBike, texTruck;
    sf::Sprite       spBg, spLogo, spCar, spBike, spTruck;

    ParkingLot* lot;
    Admin       admin;

    Screen      currentScreen = Screen::WELCOME;
    float       welcomeTimer = 0.f;
    sf::Clock   clock;

    InputField  loginUser, loginPass;
    string      loginError;

    InputField  parkPlate, parkOwner;
    int         parkTypeSelected = 0;
    string      parkMsg;
    bool        parkSuccess = false;

    InputField  exitId;
    string      exitMsg;
    bool        exitSuccess = false;
    string      exitReceiptLines[12];
    int         exitReceiptCount = 0;

    InputField  rateCar, rateBike, rateTruck;
    string      rateMsg;

    InputField  slotTypeInput, slotIdInput;
    string      slotMsg;

    InputField  findPlate;
    string      findResult;
    bool        findSuccess = false;

    int         reportPeriod = 0;
    string      reportContent;
    bool        reportGenerated = false;

    InputField  maintenanceSlotId;
    string      maintenanceMsg;

    float       historyScroll = 0.f;
    float       parkedScroll = 0.f;
    float       gridScroll = 0.f;

    bool        clickThisFrame = false;
    sf::Event   lastEvent;

    GrandValetApp() : window(sf::VideoMode(1280, 720), "Grand Valet Parking",
        sf::Style::Titlebar | sf::Style::Close) {
        window.setFramerateLimit(60);
        font.loadFromFile("assets/fonts/arial.ttf");
        texBg.loadFromFile("assets/images/background.jpg");
        texLogo.loadFromFile("assets/images/logo.png");
        texCar.loadFromFile("assets/images/car.png");
        texBike.loadFromFile("assets/images/bike.png");
        texTruck.loadFromFile("assets/images/truck.png");
        texBg.setSmooth(true); texLogo.setSmooth(true);
        texCar.setSmooth(true); texBike.setSmooth(true); texTruck.setSmooth(true);
        spBg.setTexture(texBg); spLogo.setTexture(texLogo);
        spCar.setTexture(texCar); spBike.setTexture(texBike); spTruck.setTexture(texTruck);
        float bgScX = 1280.f / texBg.getSize().x;
        float bgScY = 720.f / texBg.getSize().y;
        spBg.setScale(bgScX, bgScY);
        float logoH = 80.f;
        float logoSc = logoH / texLogo.getSize().y;
        spLogo.setScale(logoSc, logoSc);
        auto scaleSprite = [](sf::Sprite& sp, sf::Texture& tx, float targetW) {
            float sc = targetW / tx.getSize().x;
            sp.setScale(sc, sc);
            };
        scaleSprite(spCar, texCar, 200.f);
        scaleSprite(spBike, texBike, 200.f);
        scaleSprite(spTruck, texTruck, 200.f);
        lot = new ParkingLot(50);
        FileHandler::loadRates(lot);
        FileHandler::loadSlots(lot);
        initInputFields();
    }

    ~GrandValetApp() {
        FileHandler::saveSlots(lot);
        FileHandler::saveRates(lot);
        delete lot;
    }

    void initInputFields() {
        loginUser = { {440,300,400,44}, "", "Username", false, false };
        loginPass = { {440,362,400,44}, "", "Password", false, true };
        parkPlate = { {440,260,380,44}, "", "e.g. ABC-1234", false, false };
        parkOwner = { {440,322,380,44}, "", "Owner name",    false, false };
        exitId = { {440,290,400,44}, "", "Entry ID",  false, false };
        rateCar = { {620,220,160,40}, to_string((int)lot->getRate("Car")),   "Rate", false, false };
        rateBike = { {620,280,160,40}, to_string((int)lot->getRate("Bike")),  "Rate", false, false };
        rateTruck = { {620,340,160,40}, to_string((int)lot->getRate("Truck")), "Rate", false, false };
        slotTypeInput = { {440,260,200,44}, "Compact", "Compact/Large/Motorcycle/Electric", false, false };
        slotIdInput = { {440,330,200,44}, "",        "Slot ID to remove", false, false };
        findPlate = { {440,260,380,44}, "", "Enter vehicle number", false, false };
        maintenanceSlotId = { {440,260,200,44}, "", "Slot ID", false, false };
    }

    void run() {
        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();
            clickThisFrame = false;
            sf::Event ev;
            while (window.pollEvent(ev)) {
                lastEvent = ev;
                if (ev.type == sf::Event::Closed) window.close();
                if (ev.type == sf::Event::MouseButtonPressed &&
                    ev.mouseButton.button == sf::Mouse::Left)
                    clickThisFrame = true;
                handleTextInput(ev);
                handleScroll(ev);
            }
            if (currentScreen == Screen::WELCOME) welcomeTimer += dt;
            window.clear(C_BG);
            drawCurrentScreen();
            window.display();
        }
    }

    void handleTextInput(const sf::Event& ev) {
        if (ev.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f m = { (float)ev.mouseButton.x, (float)ev.mouseButton.y };
            for (auto* f : getAllInputFields()) f->active = false;
            for (auto* f : getAllInputFields()) {
                if (f->bounds.contains(m)) { f->active = true; break; }
            }
        }
        if (ev.type == sf::Event::TextEntered) {
            for (auto* f : getAllInputFields()) {
                if (!f->active) continue;
                if (ev.text.unicode == 8 && !f->value.empty())
                    f->value.pop_back();
                else if (ev.text.unicode >= 32 && ev.text.unicode < 128)
                    f->value += (char)ev.text.unicode;
            }
        }
    }

    vector<InputField*> getAllInputFields() {
        return { &loginUser, &loginPass, &parkPlate, &parkOwner,
                 &exitId, &rateCar, &rateBike, &rateTruck,
                 &slotTypeInput, &slotIdInput, &findPlate, &maintenanceSlotId };
    }

    void handleScroll(const sf::Event& ev) {
        if (ev.type == sf::Event::MouseWheelScrolled) {
            float d = ev.mouseWheelScroll.delta * 20.f;
            if (currentScreen == Screen::VIEW_HISTORY)   historyScroll -= d;
            if (currentScreen == Screen::VIEW_PARKED)    parkedScroll -= d;
            if (currentScreen == Screen::SLOT_GRID)      gridScroll -= d;
            if (historyScroll < 0) historyScroll = 0;
            if (parkedScroll < 0) parkedScroll = 0;
            if (gridScroll < 0) gridScroll = 0;
        }
    }

    void drawCurrentScreen() {
        switch (currentScreen) {
        case Screen::WELCOME:           drawWelcome();          break;
        case Screen::ROLE_SELECT:       drawRoleSelect();       break;
        case Screen::ADMIN_LOGIN:       drawAdminLogin();       break;
        case Screen::ADMIN_DASHBOARD:   drawAdminDashboard();   break;
        case Screen::USER_DASHBOARD:    drawUserDashboard();    break;
        case Screen::PARK_VEHICLE:      drawParkVehicle();      break;
        case Screen::EXIT_VEHICLE:      drawExitVehicle();      break;
        case Screen::SLOT_GRID:         drawSlotGrid();         break;
        case Screen::RATE_MANAGEMENT:   drawRateManagement();   break;
        case Screen::VIEW_HISTORY:      drawViewHistory();      break;
        case Screen::VIEW_PARKED:       drawViewParked();       break;
        case Screen::MANAGE_SLOTS:      drawManageSlots();      break;
        case Screen::FIND_VEHICLE:      drawFindVehicle();      break;
        case Screen::GENERATE_REPORT:   drawGenerateReport();   break;
        case Screen::SLOT_MAINTENANCE:  drawSlotMaintenance();  break;
        }
    }

    void drawSidebar(Screen active) {
        sf::RectangleShape sb({ 220.f, 720.f });
        sb.setPosition(0, 0); sb.setFillColor(C_SIDEBAR); window.draw(sb);
        drawDivider(window, 219, 0, 1, 720);
        spLogo.setPosition(20, 16);
        spLogo.setColor(C_GOLD);
        window.draw(spLogo);
        float logoW = texLogo.getSize().x * spLogo.getScale().x;
        drawText(window, font, "Grand Valet", 20 + logoW + 8, 22, 12, C_GOLD, true);
        drawText(window, font, "PARKING", 20 + logoW + 8, 38, 10, C_MUTED);
        drawDivider(window, 16, 110, 188);

        struct NavItem { string label; Screen screen; };
        NavItem items[] = {
            { "Dashboard",       Screen::ADMIN_DASHBOARD },
            { "Park Vehicle",    Screen::PARK_VEHICLE    },
            { "Exit Vehicle",    Screen::EXIT_VEHICLE    },
            { "Slot Grid",       Screen::SLOT_GRID       },
            { "Parked Vehicles", Screen::VIEW_PARKED     },
            { "History",         Screen::VIEW_HISTORY    },
            { "Reports",         Screen::GENERATE_REPORT },
            { "Rates",           Screen::RATE_MANAGEMENT },
            { "Manage Slots",    Screen::MANAGE_SLOTS    },
            { "Maintenance",     Screen::SLOT_MAINTENANCE },
        };
        for (int i = 0; i < 10; i++) {
            bool isActive = (currentScreen == items[i].screen);
            if (drawNavBtn(window, font, 0, 122 + i * 50, 220,
                items[i].label, isActive, lastEvent, clickThisFrame))
                currentScreen = items[i].screen;
        }
        drawDivider(window, 16, 122 + 10 * 50 + 5, 188);

        Button logoutBtn = { {16, 680, 188, 32}, "Logout", sf::Color(60,20,20), C_ERROR };
        if (drawButton(window, font, logoutBtn, lastEvent, clickThisFrame)) {
            admin.logout();
            currentScreen = Screen::ROLE_SELECT;
        }
    }

    // ??? WELCOME ??????????????????????????????????????????????????
    void drawWelcome() {
        spBg.setColor(sf::Color(100, 100, 100, 200));
        window.draw(spBg);
        sf::RectangleShape ov({ 1280.f, 720.f });
        ov.setFillColor(sf::Color(5, 10, 25, 200)); window.draw(ov);
        sf::RectangleShape topLine({ 1280.f, 3.f });
        topLine.setPosition(0, 0); topLine.setFillColor(C_GOLD); window.draw(topLine);
        topLine.setPosition(0, 717); window.draw(topLine);
        float lW = texLogo.getSize().x * spLogo.getScale().x;
        spLogo.setColor(C_GOLD); spLogo.setPosition(640 - lW / 2, 210); window.draw(spLogo);
        drawText(window, font, "GRAND VALET PARKING", 640, 315, 36, C_GOLD, true, true);
        drawDivider(window, 440, 358, 400, 1.f);
        drawText(window, font, "Premium Parking Management System", 640, 370, 14, C_MUTED, false, true);
        drawText(window, font, "v1.0  |  OOP Spring 2026", 640, 395, 11, sf::Color(80, 75, 65), false, true);
        int dots = (int)(welcomeTimer * 3) % 4;
        drawText(window, font, "Loading" + string(dots, '.'), 640, 460, 13, C_MUTED, false, true);
        float prog = min(welcomeTimer / 3.0f, 1.0f);
        sf::RectangleShape pbg({ 300.f, 3.f }); pbg.setPosition(490, 490); pbg.setFillColor(sf::Color(30, 35, 60)); window.draw(pbg);
        sf::RectangleShape pfg({ 300.f * prog, 3.f }); pfg.setPosition(490, 490); pfg.setFillColor(C_GOLD); window.draw(pfg);
        if (welcomeTimer >= 3.0f) currentScreen = Screen::ROLE_SELECT;
    }

    // ??? ROLE SELECT ??????????????????????????????????????????????
    void drawRoleSelect() {
        spBg.setColor(sf::Color(100, 100, 100, 255)); window.draw(spBg);
        sf::RectangleShape ov({ 1280.f, 720.f });
        ov.setFillColor(sf::Color(5, 10, 25, 185)); window.draw(ov);
        drawRoundRect(window, 60, 80, 520, 560, 12.f, sf::Color(8, 14, 30, 220), C_GOLD_DARK, 1.f);
        sf::RectangleShape topAccent({ 520.f, 4.f }); topAccent.setPosition(60, 80); topAccent.setFillColor(C_GOLD); window.draw(topAccent);
        float lW = texLogo.getSize().x * spLogo.getScale().x;
        spLogo.setColor(C_GOLD); spLogo.setPosition(60 + 260 - lW / 2, 110); window.draw(spLogo);
        drawText(window, font, "GRAND VALET", 320, 205, 22, C_GOLD, true, true);
        drawText(window, font, "PARKING SYSTEM", 320, 232, 13, C_MUTED, false, true);
        drawDivider(window, 100, 260, 440);
        float carW = texCar.getSize().x * spCar.getScale().x;
        float carH = texCar.getSize().y * spCar.getScale().y;
        spCar.setPosition(60 + 260 - carW / 2, 280); spCar.setColor(sf::Color::White); window.draw(spCar);
        drawText(window, font, "50 Premium Parking Slots", 320, 290 + carH + 10, 12, C_MUTED, false, true);
        drawText(window, font, "Available: " + to_string(lot->getAvailableCount()), 320, 308 + carH + 10, 14, C_GOLD, true, true);
        drawRoundRect(window, 640, 80, 580, 560, 12.f, sf::Color(8, 14, 30, 220), C_GOLD_DARK, 1.f);
        topAccent.setPosition(640, 80); window.draw(topAccent);
        drawText(window, font, "Select Your Role", 930, 130, 20, C_WHITE, true, true);
        drawText(window, font, "Choose how you'd like to proceed", 930, 160, 12, C_MUTED, false, true);
        drawDivider(window, 680, 185, 500);
        drawRoundRect(window, 680, 210, 500, 140, 10.f, sf::Color(15, 22, 50), C_GOLD_DARK, 1.f);
        sf::RectangleShape adminAccent({ 4.f, 100.f }); adminAccent.setPosition(680, 245); adminAccent.setFillColor(C_GOLD); window.draw(adminAccent);
        drawText(window, font, "ADMIN", 710, 225, 18, C_GOLD, true);
        drawText(window, font, "Manage slots, rates, reports", 710, 250, 12, C_MUTED);
        drawText(window, font, "& view parking history", 710, 268, 12, C_MUTED);
        Button adminBtn = { {710, 290, 430, 40}, "Login as Admin", C_GOLD_DARK, C_WHITE };
        if (drawButton(window, font, adminBtn, lastEvent, clickThisFrame)) {
            loginUser.value = ""; loginPass.value = ""; loginError = "";
            currentScreen = Screen::ADMIN_LOGIN;
        }
        drawRoundRect(window, 680, 370, 500, 140, 10.f, sf::Color(15, 22, 50), sf::Color(40, 60, 100), 1.f);
        sf::RectangleShape userAccent({ 4.f, 100.f }); userAccent.setPosition(680, 405); userAccent.setFillColor(sf::Color(80, 120, 200)); window.draw(userAccent);
        drawText(window, font, "USER", 710, 385, 18, sf::Color(120, 160, 230), true);
        drawText(window, font, "Park your vehicle, get billing", 710, 410, 12, C_MUTED);
        drawText(window, font, "and view slot availability", 710, 428, 12, C_MUTED);
        Button userBtn = { {710, 450, 430, 40}, "Enter as User", sf::Color(20,40,100), C_WHITE };
        if (drawButton(window, font, userBtn, lastEvent, clickThisFrame)) {
            parkPlate.value = ""; parkOwner.value = "";
            parkMsg = ""; parkSuccess = false; parkTypeSelected = 0;
            currentScreen = Screen::USER_DASHBOARD;
        }
        drawDivider(window, 680, 535, 500);
        drawText(window, font, "Total: " + to_string(lot->getTotalSlots()) + " slots", 720, 548, 12, C_MUTED);
        drawText(window, font, "Occupied: " + to_string(lot->getTotalSlots() - lot->getAvailableCount()), 880, 548, 12, C_MUTED);
        drawText(window, font, "Free: " + to_string(lot->getAvailableCount()), 1040, 548, 12, C_SUCCESS);
    }

    // ??? ADMIN LOGIN ??????????????????????????????????????????????
    void drawAdminLogin() {
        window.clear(C_BG);
        spBg.setColor(sf::Color(60, 60, 60, 150)); window.draw(spBg);
        sf::RectangleShape ov({ 1280.f, 720.f }); ov.setFillColor(sf::Color(5, 10, 25, 210)); window.draw(ov);
        drawRoundRect(window, 390, 120, 500, 480, 12.f, sf::Color(10, 18, 40, 240), C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ 500.f, 4.f }); topAcc.setPosition(390, 120); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        float lW = texLogo.getSize().x * spLogo.getScale().x;
        spLogo.setColor(C_GOLD); spLogo.setPosition(640 - lW / 2, 138); window.draw(spLogo);
        drawText(window, font, "ADMIN LOGIN", 640, 232, 20, C_GOLD, true, true);
        drawText(window, font, "Enter your credentials to continue", 640, 258, 12, C_MUTED, false, true);
        drawDivider(window, 410, 282, 460);
        drawText(window, font, "USERNAME", 440, 298, 11, C_MUTED); drawInput(window, font, loginUser);
        drawText(window, font, "PASSWORD", 440, 358, 11, C_MUTED); drawInput(window, font, loginPass);
        if (!loginError.empty()) drawText(window, font, loginError, 640, 418, 12, C_ERROR, false, true);
        Button loginBtn = { {440, 435, 400, 46}, "LOGIN", C_GOLD_DARK, C_WHITE };
        if (drawButton(window, font, loginBtn, lastEvent, clickThisFrame)) {
            if (admin.login(loginUser.value, loginPass.value)) { loginError = ""; currentScreen = Screen::ADMIN_DASHBOARD; }
            else loginError = "Invalid username or password. Try again.";
        }
        Button backBtn = { {440, 495, 400, 36}, "Back", sf::Color(20,28,55), C_MUTED };
        if (drawButton(window, font, backBtn, lastEvent, clickThisFrame)) currentScreen = Screen::ROLE_SELECT;
        drawText(window, font, "Default: admin / admin123", 640, 550, 11, sf::Color(60, 55, 50), false, true);
    }

    // ??? ADMIN DASHBOARD ??????????????????????????????????????????
    void drawAdminDashboard() {
        window.clear(C_BG); drawSidebar(Screen::ADMIN_DASHBOARD); drawHeader(window, font, "Dashboard", 220, 1060);
        int total = lot->getTotalSlots(), avail = lot->getAvailableCount(), occupied = total - avail;
        drawStatCard(window, font, 240, 80, 220, 90, "Total Slots", to_string(total));
        drawStatCard(window, font, 480, 80, 220, 90, "Available", to_string(avail));
        drawStatCard(window, font, 720, 80, 220, 90, "Occupied", to_string(occupied));
        drawStatCard(window, font, 960, 80, 280, 90, "Car Rate (Rs/min)", to_string((int)lot->getRate("Car")));
        drawDivider(window, 240, 188, 1020);
        drawSectionHeading(window, font, "Slot Overview", 240, 200, 400);
        int cols = 16, cellW = 58, cellH = 36, gapX = 5, gapY = 5;
        int previewCount = min(total, 48);
        for (int i = 0; i < previewCount; i++) {
            int row = i / cols, col = i % cols;
            float cx = 240 + col * (cellW + gapX), cy = 232 + row * (cellH + gapY);
            ParkingSlot* s = lot->getSlot(i);
            sf::Color fill = C_SLOT_FREE;
            if (s) {
                if (s->getStatus() == "occupied") fill = C_SLOT_BUSY;
                else if (s->getStatus() == "maintenance") fill = C_SLOT_MAINT;
            }
            drawRoundRect(window, cx, cy, cellW, cellH, 4.f, fill, sf::Color::Transparent);
            drawText(window, font, to_string(i), cx + 2, cy + 2, 8, sf::Color(0, 0, 0, 180));
        }
        float ly = 232 + 3 * (cellH + gapY) + 10;
        auto legend = [&](float x, sf::Color c, const string& l) {
            sf::RectangleShape r({ 12.f,12.f }); r.setPosition(x, ly + 2); r.setFillColor(c); window.draw(r);
            drawText(window, font, l, x + 16, ly, 11, C_MUTED);
            };
        legend(240, C_SLOT_FREE, "Available"); legend(340, C_SLOT_BUSY, "Occupied"); legend(440, C_SLOT_MAINT, "Maintenance");
        drawSectionHeading(window, font, "Quick Actions", 240, ly + 30, 400);
        Button b1 = { {240, ly + 58, 180, 40}, "Park Vehicle",  C_GOLD_DARK,         C_WHITE };
        Button b2 = { {436, ly + 58, 180, 40}, "Exit Vehicle",  sf::Color(30,60,100), C_WHITE };
        Button b3 = { {632, ly + 58, 180, 40}, "Slot Grid",     sf::Color(20,50,40),  C_WHITE };
        Button b4 = { {828, ly + 58, 180, 40}, "View History",  sf::Color(60,30,20),  C_WHITE };
        if (drawButton(window, font, b1, lastEvent, clickThisFrame)) { parkMsg = ""; parkSuccess = false; currentScreen = Screen::PARK_VEHICLE; }
        if (drawButton(window, font, b2, lastEvent, clickThisFrame)) { exitMsg = ""; exitSuccess = false; exitId.value = ""; currentScreen = Screen::EXIT_VEHICLE; }
        if (drawButton(window, font, b3, lastEvent, clickThisFrame)) currentScreen = Screen::SLOT_GRID;
        if (drawButton(window, font, b4, lastEvent, clickThisFrame)) { historyScroll = 0; currentScreen = Screen::VIEW_HISTORY; }
    }

    // ??? USER DASHBOARD ???????????????????????????????????????????
    void drawUserDashboard() {
        window.clear(C_BG); spBg.setColor(sf::Color(60, 60, 60, 150)); window.draw(spBg);
        sf::RectangleShape ov({ 1280.f, 720.f }); ov.setFillColor(sf::Color(5, 10, 25, 210)); window.draw(ov);
        drawRoundRect(window, 240, 50, 800, 620, 12.f, C_PANEL, C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ 800.f, 4.f }); topAcc.setPosition(240, 50); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        float lW = texLogo.getSize().x * spLogo.getScale().x;
        spLogo.setColor(C_GOLD); spLogo.setPosition(640 - lW / 2, 66); window.draw(spLogo);
        drawText(window, font, "USER MENU", 640, 148, 20, C_GOLD, true, true);
        drawText(window, font, "Select an option below", 640, 174, 12, C_MUTED, false, true);
        drawDivider(window, 280, 196, 720);
        int total = lot->getTotalSlots(), avail = lot->getAvailableCount(), occupied = total - avail;
        drawStatCard(window, font, 280, 216, 220, 80, "Total Slots", to_string(total));
        drawStatCard(window, font, 520, 216, 220, 80, "Available", to_string(avail));
        drawStatCard(window, font, 760, 216, 220, 80, "Occupied", to_string(occupied));
        drawDivider(window, 280, 316, 720);
        float bx = 340, bw = 600, bh = 50, by = 340;
        Button parkBtn = { {bx, by, bw, bh}, "PARK MY VEHICLE",           C_GOLD_DARK,         C_WHITE };
        Button exitBtn = { {bx, by + 65, bw, bh}, "EXIT MY VEHICLE (Get Bill)", sf::Color(30,60,100), C_WHITE };
        Button gridBtn = { {bx, by + 130, bw, bh}, "VIEW SLOT AVAILABILITY",   sf::Color(20,50,40),  C_WHITE };
        Button findBtn = { {bx, by + 195, bw, bh}, "FIND MY VEHICLE",          sf::Color(60,40,20),  C_WHITE };
        Button backBtn = { {bx, by + 260, bw, bh}, "BACK TO MAIN MENU",        sf::Color(30,30,60),  C_MUTED };
        if (drawButton(window, font, parkBtn, lastEvent, clickThisFrame)) { parkPlate.value = ""; parkOwner.value = ""; parkMsg = ""; parkSuccess = false; parkTypeSelected = 0; currentScreen = Screen::PARK_VEHICLE; }
        if (drawButton(window, font, exitBtn, lastEvent, clickThisFrame)) { exitId.value = ""; exitMsg = ""; exitSuccess = false; exitReceiptCount = 0; currentScreen = Screen::EXIT_VEHICLE; }
        if (drawButton(window, font, gridBtn, lastEvent, clickThisFrame)) { gridScroll = 0; currentScreen = Screen::SLOT_GRID; }
        if (drawButton(window, font, findBtn, lastEvent, clickThisFrame)) { findPlate.value = ""; findResult = ""; findSuccess = false; currentScreen = Screen::FIND_VEHICLE; }
        if (drawButton(window, font, backBtn, lastEvent, clickThisFrame)) currentScreen = Screen::ROLE_SELECT;
        drawText(window, font, "Walk-in user  o  No registration required", 640, 642, 11, sf::Color(60, 55, 50), false, true);
    }

    // ??? PARK VEHICLE ?????????????????????????????????????????????
    void drawParkVehicle() {
        window.clear(C_BG);
        bool isAdmin = admin.getLoginStatus();
        if (isAdmin) drawSidebar(Screen::PARK_VEHICLE);
        float cx = isAdmin ? 220.f : 0.f, cw = isAdmin ? 1060.f : 1280.f;
        if (isAdmin) drawHeader(window, font, "Park Vehicle", cx, cw);
        float panelX = cx + (cw - 700) / 2;
        drawRoundRect(window, panelX, 80, 700, 530, 12.f, C_PANEL, C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ 700.f, 4.f }); topAcc.setPosition(panelX, 80); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        drawText(window, font, "PARK A VEHICLE", panelX + 350, 105, 18, C_GOLD, true, true);
        drawDivider(window, panelX + 20, 132, 660);
        drawText(window, font, "VEHICLE TYPE", panelX + 20, 148, 11, C_MUTED);
        int newSel = drawTypeSelector(window, font, panelX + 20, 168, parkTypeSelected, lastEvent, clickThisFrame);
        if (newSel != parkTypeSelected) { parkTypeSelected = newSel; parkMsg = ""; }
        sf::Sprite* sp = (parkTypeSelected == 0) ? &spCar : (parkTypeSelected == 1) ? &spBike : &spTruck;
        float imgW = (parkTypeSelected == 0) ? texCar.getSize().x * spCar.getScale().x
            : (parkTypeSelected == 1) ? texBike.getSize().x * spBike.getScale().x : texTruck.getSize().x * spTruck.getScale().x;
        sp->setPosition(panelX + 700 - imgW - 20, 160); sp->setColor(sf::Color::White); window.draw(*sp);
        drawText(window, font, "VEHICLE NUMBER / PLATE", panelX + 20, 222, 11, C_MUTED);
        parkPlate.bounds = { panelX + 20, 240, 440, 44 }; drawInput(window, font, parkPlate);
        drawText(window, font, "OWNER NAME", panelX + 20, 298, 11, C_MUTED);
        parkOwner.bounds = { panelX + 20, 316, 440, 44 }; drawInput(window, font, parkOwner);
        string slotTypeInfo = (parkTypeSelected == 0) ? "Will use: Compact / Large / Electric slot"
            : (parkTypeSelected == 1) ? "Will use: Motorcycle slot" : "Will use: Large slot";
        drawText(window, font, slotTypeInfo, panelX + 20, 374, 11, C_MUTED);
        drawText(window, font, "Available slots: " + to_string(lot->getAvailableCount()), panelX + 20, 392, 11, C_MUTED);
        if (!parkMsg.empty()) drawText(window, font, parkMsg, panelX + 350, 418, 13, parkSuccess ? C_SUCCESS : C_ERROR, false, true);
        Button assignBtn = { {panelX + 20, 440, 320, 46}, "ASSIGN SLOT", C_GOLD_DARK, C_WHITE };
        Button clearBtn = { {panelX + 360, 440, 150, 46}, "Clear", sf::Color(30,35,65), C_MUTED };
        Button backBtn = { {panelX + 525, 440, 155, 46}, "Back",  sf::Color(30,35,65), C_MUTED };
        if (drawButton(window, font, assignBtn, lastEvent, clickThisFrame)) {
            if (parkPlate.value.empty() || parkOwner.value.empty()) { parkMsg = "Please fill in all fields."; parkSuccess = false; }
            else {
                string types[3] = { "Car","Bike","Truck" }; Vehicle* v = nullptr; string t = types[parkTypeSelected];
                if (t == "Car") v = new Car(parkPlate.value, parkOwner.value);
                else if (t == "Bike") v = new Bike(parkPlate.value, parkOwner.value);
                else v = new Truck(parkPlate.value, parkOwner.value);
                v->generateEntryId();
                ParkingSlot* s = lot->assignSlot(v);
                if (s) { parkMsg = "SUCCESS!  Entry ID: " + v->getEntryId() + "   Slot: " + to_string(s->getSlotId()); parkSuccess = true; parkPlate.value = ""; parkOwner.value = ""; }
                else { parkMsg = "No suitable slot available."; parkSuccess = false; delete v; }
            }
        }
        if (drawButton(window, font, clearBtn, lastEvent, clickThisFrame)) { parkPlate.value = ""; parkOwner.value = ""; parkMsg = ""; parkSuccess = false; }
        if (drawButton(window, font, backBtn, lastEvent, clickThisFrame)) currentScreen = isAdmin ? Screen::ADMIN_DASHBOARD : Screen::USER_DASHBOARD;
        if (parkSuccess && !parkMsg.empty()) {
            drawRoundRect(window, panelX + 20, 498, 660, 98, 8.f, sf::Color(15, 40, 25), C_SUCCESS, 1.f);
            drawText(window, font, "Vehicle Parked Successfully", panelX + 350, 510, 13, C_SUCCESS, true, true);
            drawText(window, font, parkMsg.substr(parkMsg.find("Entry")), panelX + 350, 532, 12, C_WHITE, false, true);
            drawText(window, font, "Please note your Entry ID for exit", panelX + 350, 552, 11, C_MUTED, false, true);
        }
    }

    // ??? EXIT VEHICLE ?????????????????????????????????????????????
    void drawExitVehicle() {
        window.clear(C_BG);
        bool isAdmin = admin.getLoginStatus();
        if (isAdmin) drawSidebar(Screen::EXIT_VEHICLE);
        float cx = isAdmin ? 220.f : 0.f, cw = isAdmin ? 1060.f : 1280.f;
        if (isAdmin) drawHeader(window, font, "Exit Vehicle", cx, cw);
        float panelX = cx + (cw - 700) / 2;
        drawRoundRect(window, panelX, 80, 700, 560, 12.f, C_PANEL, C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ 700.f, 4.f }); topAcc.setPosition(panelX, 80); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        drawText(window, font, "VEHICLE EXIT", panelX + 350, 105, 18, C_GOLD, true, true);
        drawDivider(window, panelX + 20, 132, 660);
        drawText(window, font, "ENTRY ID", panelX + 20, 148, 11, C_MUTED);
        exitId.bounds = { panelX + 20, 166, 540, 44 }; drawInput(window, font, exitId);
        Button retrieveBtn = { {panelX + 20, 226, 260, 44}, "RETRIEVE RECORD", C_GOLD_DARK, C_WHITE };
        Button clearBtn2 = { {panelX + 296, 226, 120, 44}, "Clear", sf::Color(30,35,65), C_MUTED };
        Button backBtn2 = { {panelX + 432, 226, 120, 44}, "Back",  sf::Color(30,35,65), C_MUTED };
        if (drawButton(window, font, retrieveBtn, lastEvent, clickThisFrame)) {
            exitMsg = ""; exitSuccess = false; exitReceiptCount = 0;
            if (exitId.value.empty()) { exitMsg = "Please enter an Entry ID."; }
            else {
                ParkingSlot* slot = findSlotByEntryId(lot, exitId.value);
                if (!slot) { exitMsg = "Entry ID not found."; }
                else {
                    Vehicle* v = slot->getParkedVehicle();
                    time_t exitTime = time(nullptr);
                    float rate = lot->getRate(v->getType());
                    Bill bill(v, exitTime, rate); bill.calculate(); bill.exportToFile();
                    exitReceiptLines[0] = "Entry ID    : " + bill.getEntryId();
                    exitReceiptLines[1] = "Vehicle No. : " + bill.getVehicleNo();
                    exitReceiptLines[2] = "Owner       : " + bill.getOwnerName();
                    exitReceiptLines[3] = "Type        : " + bill.getVehicleType();
                    exitReceiptLines[4] = "Entry Time  : " + timeToStr(bill.getEntryTime());
                    exitReceiptLines[5] = "Exit Time   : " + timeToStr(bill.getExitTime());
                    exitReceiptLines[6] = "Duration    : " + to_string(bill.getDuration()) + " minutes";
                    exitReceiptLines[7] = "Rate        : Rs " + to_string((int)rate) + " / minute";
                    ostringstream oss; oss << fixed << setprecision(0) << bill.getAmount();
                    exitReceiptLines[8] = "TOTAL DUE   : Rs " + oss.str();
                    exitReceiptCount = 9; exitSuccess = true;
                    exitMsg = "Vehicle exited. Receipt generated.";
                    Vehicle* vacatedVehicle = slot->vacate();
                    if (vacatedVehicle) delete vacatedVehicle;
                }
            }
        }
        if (drawButton(window, font, clearBtn2, lastEvent, clickThisFrame)) { exitId.value = ""; exitMsg = ""; exitSuccess = false; exitReceiptCount = 0; }
        if (drawButton(window, font, backBtn2, lastEvent, clickThisFrame)) currentScreen = isAdmin ? Screen::ADMIN_DASHBOARD : Screen::USER_DASHBOARD;
        if (!exitMsg.empty()) drawText(window, font, exitMsg, panelX + 350, 286, 12, exitSuccess ? C_SUCCESS : C_ERROR, false, true);
        if (exitSuccess && exitReceiptCount > 0) {
            drawRoundRect(window, panelX + 20, 306, 660, 250, 8.f, sf::Color(10, 18, 35), C_GOLD_DARK, 1.f);
            sf::RectangleShape rh({ 660.f, 32.f }); rh.setPosition(panelX + 20, 306); rh.setFillColor(sf::Color(30, 22, 8)); window.draw(rh);
            drawText(window, font, "PARKING RECEIPT", panelX + 350, 313, 13, C_GOLD, true, true);
            for (int i = 0; i < exitReceiptCount; i++) {
                sf::Color col = (i == 8) ? C_GOLD : C_WHITE; bool bold = (i == 8);
                drawText(window, font, exitReceiptLines[i], panelX + 36, 346 + i * 22, (i == 8) ? 15u : 12u, col, bold);
            }
            drawDivider(window, panelX + 36, 344 + exitReceiptCount * 22, 628);
            drawText(window, font, "Thank you! Drive safely.", panelX + 350, 352 + exitReceiptCount * 22, 12, C_MUTED, false, true);
        }
    }

    // ??? SLOT GRID ????????????????????????????????????????????????
    void drawSlotGrid() {
        window.clear(C_BG);
        bool isAdmin = admin.getLoginStatus();
        if (isAdmin) drawSidebar(Screen::SLOT_GRID);
        float cx = isAdmin ? 220.f : 0.f, cw = isAdmin ? 1060.f : 1280.f;
        if (isAdmin) drawHeader(window, font, "Slot Grid", cx, cw);
        float startX = cx + 20, startY = 72;
        auto leg = [&](float x, float y, sf::Color c, const string& l) {
            sf::RectangleShape r({ 14.f,14.f }); r.setPosition(x, y); r.setFillColor(c); window.draw(r);
            drawText(window, font, l, x + 18, y - 1, 11, C_MUTED);
            };
        leg(startX, startY, C_SLOT_FREE, "Available"); leg(startX + 120, startY, C_SLOT_BUSY, "Occupied"); leg(startX + 240, startY, C_SLOT_MAINT, "Maintenance");
        drawText(window, font, "Free: " + to_string(lot->getAvailableCount()) + "  |  Occupied: " + to_string(lot->getTotalSlots() - lot->getAvailableCount()) + "  |  Total: " + to_string(lot->getTotalSlots()), cx + cw - 380, startY, 11, C_MUTED);
        drawDivider(window, cx + 10, startY + 20, cw - 20);
        int cols = 10, cellW = 90, cellH = 52, gapX = 8, gapY = 8;
        float gridY = startY + 32 - gridScroll;
        for (int i = 0; i < lot->getTotalSlots(); i++) {
            int row = i / cols, col = i % cols;
            float gx = startX + col * (cellW + gapX), gy = gridY + row * (cellH + gapY);
            if (gy + cellH < startY + 24 || gy > 720) continue;
            ParkingSlot* s = lot->getSlot(i);
            sf::Color fill = C_SLOT_FREE; string typeLabel = "Empty", statusLabel = "FREE";
            if (s) {
                typeLabel = s->getType().substr(0, 3);
                if (s->getStatus() == "occupied") { fill = C_SLOT_BUSY; statusLabel = "BUSY"; }
                else if (s->getStatus() == "maintenance") { fill = C_SLOT_MAINT; statusLabel = "MNTN"; }
                else statusLabel = "FREE";
            }
            drawRoundRect(window, gx, gy, cellW, cellH, 5.f, sf::Color(fill.r / 4 + 8, fill.g / 4 + 12, fill.b / 4 + 20), fill, 1.5f);
            sf::RectangleShape top({ (float)cellW, 4.f }); top.setPosition(gx, gy); top.setFillColor(fill); window.draw(top);
            drawText(window, font, to_string(i), gx + 4, gy + 4, 9, C_MUTED);
            drawText(window, font, typeLabel, gx + cellW / 2, gy + 18, 11, C_WHITE, false, true);
            drawText(window, font, statusLabel, gx + cellW / 2, gy + 34, 9, fill, true, true);
        }
        Button backBtn = { {cx + cw - 150, 682, 130, 32}, "Back", sf::Color(20,28,55), C_MUTED };
        if (drawButton(window, font, backBtn, lastEvent, clickThisFrame)) currentScreen = isAdmin ? Screen::ADMIN_DASHBOARD : Screen::USER_DASHBOARD;
    }

    // ??? RATE MANAGEMENT ??????????????????????????????????????????
    void drawRateManagement() {
        window.clear(C_BG); drawSidebar(Screen::RATE_MANAGEMENT); drawHeader(window, font, "Rate Management", 220, 1060);
        float px = 280, py = 78, panelW = 720;
        drawRoundRect(window, px, py, panelW, 540, 12.f, C_PANEL, C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ panelW, 4.f }); topAcc.setPosition(px, py); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        drawText(window, font, "PARKING RATES", px + panelW / 2, py + 24, 18, C_GOLD, true, true);
        drawText(window, font, "Set rate per minute (Rs) per vehicle type", px + panelW / 2, py + 50, 11, C_MUTED, false, true);
        drawDivider(window, px + 20, py + 72, panelW - 40);
        float rowStartX = px + 30, rowW = panelW - 60, rowH = 110, rowGap = 16;
        float accentW = 5.f, leftColX = rowStartX + accentW + 24;
        float midColX = rowStartX + 220, inputX = rowStartX + 420, btnX = rowStartX + 550;
        struct RowInfo { string label; string subtitle; string type; InputField* inp; sf::Color accent; };
        RowInfo rows[3] = {
            { "CAR", "Compact / Large / Electric", "Car", &rateCar, C_GOLD },
            { "BIKE", "Motorcycle slot", "Bike", &rateBike, sf::Color(80, 160, 220) },
            { "TRUCK", "Large slot", "Truck", &rateTruck, sf::Color(200, 120, 60) }
        };
        for (int i = 0; i < 3; i++) {
            float ry = py + 90 + i * (rowH + rowGap);
            drawRoundRect(window, rowStartX, ry, rowW, rowH, 8.f, sf::Color(14, 22, 50), C_DIVIDER, 1.f);
            sf::RectangleShape bar({ accentW, rowH }); bar.setPosition(rowStartX, ry); bar.setFillColor(rows[i].accent); window.draw(bar);
            sf::RectangleShape vdiv({ 1.f, rowH - 20.f }); vdiv.setPosition(rowStartX + 200, ry + 10); vdiv.setFillColor(C_DIVIDER); window.draw(vdiv);
            drawText(window, font, rows[i].label, leftColX, ry + 22, 16, rows[i].accent, true);
            drawText(window, font, rows[i].subtitle, leftColX, ry + 46, 10, C_MUTED);
            string rateText = "Rs " + to_string((int)lot->getRate(rows[i].type)) + " / min";
            drawText(window, font, "Current:", leftColX, ry + 70, 11, C_MUTED);
            drawText(window, font, rateText, leftColX + 70, ry + 70, 12, C_WHITE, true);
            drawText(window, font, "New Rate:", midColX + 20, ry + 30, 11, C_MUTED);
            rows[i].inp->bounds = { inputX, ry + 22, 100, 40 }; drawInput(window, font, *rows[i].inp);
            drawText(window, font, "Rs/min", inputX + 112, ry + 34, 10, C_MUTED);
            Button upBtn = { {btnX, ry + 22, 110, 40}, "UPDATE", C_GOLD_DARK, C_WHITE };
            if (drawButton(window, font, upBtn, lastEvent, clickThisFrame)) {
                try {
                    float val = stof(rows[i].inp->value);
                    if (val < 0) rateMsg = "Rate cannot be negative.";
                    else { admin.updateRate(lot, rows[i].type, val); rateMsg = rows[i].label + " rate updated to Rs " + to_string((int)val) + " / min"; rows[i].inp->value = to_string((int)val); }
                }
                catch (...) { rateMsg = "Invalid value."; }
            }
        }
        float msgY = py + 90 + 3 * (rowH + rowGap) + 10;
        if (!rateMsg.empty()) { bool isErr = rateMsg.find("Invalid") != string::npos || rateMsg.find("negative") != string::npos; drawText(window, font, rateMsg, px + panelW / 2, msgY, 12, isErr ? C_ERROR : C_SUCCESS, false, true); }
        drawText(window, font, "Rates are saved automatically.", px + panelW / 2, msgY + 22, 10, C_MUTED, false, true);
    }

    // ??? VIEW HISTORY ?????????????????????????????????????????????
    void drawViewHistory() {
        window.clear(C_BG); drawSidebar(Screen::VIEW_HISTORY); drawHeader(window, font, "Parking History", 220, 1060);
        float startY = 70;
        float cols[] = { 230, 330, 500, 600, 720, 860, 960, 1100 };
        string headers[] = { "Entry ID","Plate","Owner","Type","Entry","Exit","Mins","Amount" };
        for (int i = 0; i < 8; i++) drawText(window, font, headers[i], cols[i], startY, 11, C_GOLD, true);
        drawDivider(window, 230, startY + 18, 1040);
        ifstream file("data/history.txt");
        if (!file.is_open()) { drawText(window, font, "No history records found.", 640, 200, 14, C_MUTED, false, true); return; }
        string line; int rowIdx = 0; float rowH = 24.f, clipTop = startY + 22;
        while (getline(file, line)) {
            if (line.empty()) continue;
            float ry = clipTop + rowIdx * rowH - historyScroll;
            if (ry > 700) break; if (ry < clipTop) { rowIdx++; continue; }
            sf::Color rowBg = (rowIdx % 2 == 0) ? sf::Color(14, 20, 40) : sf::Color(10, 16, 34);
            sf::RectangleShape rb({ 1040.f, rowH }); rb.setPosition(230, ry); rb.setFillColor(rowBg); window.draw(rb);
            stringstream ss(line); string tok; string parts[8]; int pi = 0;
            while (getline(ss, tok, ',') && pi < 8) parts[pi++] = tok;
            string entryStr = parts[4], exitStr = parts[5];
            try { entryStr = timeToStr((time_t)stol(parts[4])); }
            catch (...) {}
            try { exitStr = timeToStr((time_t)stol(parts[5])); }
            catch (...) {}
            string vals[] = { parts[0],parts[1],parts[2],parts[3],entryStr,exitStr,parts[6],parts[7] };
            for (int i = 0; i < 8; i++) drawText(window, font, vals[i].substr(0, 14), cols[i], ry + 5, 11, (i == 7) ? C_GOLD : C_WHITE);
            rowIdx++;
        }
        file.close();
        if (rowIdx == 0) drawText(window, font, "No records found.", 640, 200, 14, C_MUTED, false, true);
    }

    // ??? VIEW PARKED ??????????????????????????????????????????????
    void drawViewParked() {
        window.clear(C_BG); drawSidebar(Screen::VIEW_PARKED); drawHeader(window, font, "Currently Parked Vehicles", 220, 1060);
        float cols[] = { 230, 340, 500, 660, 800, 960 };
        string headers[] = { "Slot","Plate No.","Owner","Type","Entry Time","Slot Type" };
        float startY = 70;
        for (int i = 0; i < 6; i++) drawText(window, font, headers[i], cols[i], startY, 11, C_GOLD, true);
        drawDivider(window, 230, startY + 18, 1040);
        int rowIdx = 0; float rowH = 28.f, clipTop = startY + 22;
        for (int i = 0; i < lot->getTotalSlots(); i++) {
            ParkingSlot* s = lot->getSlot(i);
            if (!s || s->isAvailable()) continue;
            Vehicle* v = s->getParkedVehicle(); if (!v) continue;
            float ry = clipTop + rowIdx * rowH - parkedScroll;
            if (ry > 700) break; if (ry < clipTop) { rowIdx++; continue; }
            sf::Color rowBg = (rowIdx % 2 == 0) ? sf::Color(14, 20, 40) : sf::Color(10, 16, 34);
            sf::RectangleShape rb({ 1040.f,rowH }); rb.setPosition(230, ry); rb.setFillColor(rowBg); window.draw(rb);
            sf::CircleShape dot(4.f); dot.setFillColor(C_SLOT_BUSY); dot.setPosition(cols[0] - 14, ry + 10); window.draw(dot);
            string vals[] = { to_string(s->getSlotId()), v->getVehicleNo(), v->getOwnerName(), v->getType(), timeToStr(v->getEntryTime()), s->getType() };
            for (int j = 0; j < 6; j++) drawText(window, font, vals[j], cols[j], ry + 7, 12, C_WHITE);
            rowIdx++;
        }
        if (rowIdx == 0) drawText(window, font, "No vehicles currently parked.", 640, 200, 14, C_MUTED, false, true);
    }

    // ??? MANAGE SLOTS ?????????????????????????????????????????????
    void drawManageSlots() {
        window.clear(C_BG); drawSidebar(Screen::MANAGE_SLOTS); drawHeader(window, font, "Manage Slots", 220, 1060);
        float px = 340, py = 78;
        drawRoundRect(window, px, py, 600, 520, 12.f, C_PANEL, C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ 600.f,4.f }); topAcc.setPosition(px, py); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        drawText(window, font, "SLOT MANAGEMENT", px + 300, py + 22, 16, C_GOLD, true, true);
        drawDivider(window, px + 20, py + 48, 560);
        drawText(window, font, "Current total slots: " + to_string(lot->getTotalSlots()), px + 20, py + 62, 13, C_WHITE);
        drawText(window, font, "Available: " + to_string(lot->getAvailableCount()), px + 260, py + 62, 13, C_SUCCESS);
        drawRoundRect(window, px + 20, py + 90, 560, 160, 8.f, sf::Color(12, 20, 44), C_DIVIDER, 1.f);
        drawText(window, font, "ADD NEW SLOT", px + 40, py + 106, 13, C_GOLD, true);
        drawText(window, font, "Slot Type:", px + 40, py + 132, 12, C_MUTED);
        slotTypeInput.bounds = { px + 130, py + 126, 280, 38 }; drawInput(window, font, slotTypeInput);
        drawText(window, font, "Options: Compact | Large | Motorcycle | Electric", px + 40, py + 172, 10, C_MUTED);
        Button addBtn = { {px + 430, py + 126, 130, 38}, "ADD SLOT", C_SUCCESS, C_WHITE };
        if (drawButton(window, font, addBtn, lastEvent, clickThisFrame)) {
            string t = slotTypeInput.value;
            if (t != "Compact" && t != "Large" && t != "Motorcycle" && t != "Electric") slotMsg = "Invalid type.";
            else { lot->addSlot(t); slotMsg = "Slot added. Total: " + to_string(lot->getTotalSlots()); slotTypeInput.value = ""; }
        }
        drawRoundRect(window, px + 20, py + 270, 560, 160, 8.f, sf::Color(12, 20, 44), C_DIVIDER, 1.f);
        drawText(window, font, "REMOVE SLOT", px + 40, py + 286, 13, C_ERROR, true);
        drawText(window, font, "Only the last slot can be removed if empty.", px + 40, py + 308, 11, C_MUTED);
        drawText(window, font, "Last slot ID: " + to_string(lot->getTotalSlots() - 1), px + 40, py + 326, 11, C_MUTED);
        drawText(window, font, "Slot ID:", px + 40, py + 350, 12, C_MUTED);
        slotIdInput.bounds = { px + 120, py + 344, 200, 38 }; drawInput(window, font, slotIdInput);
        Button removeBtn = { {px + 340, py + 344, 130, 38}, "REMOVE", sf::Color(100,20,20), C_WHITE };
        if (drawButton(window, font, removeBtn, lastEvent, clickThisFrame)) {
            try { int id = stoi(slotIdInput.value); if (lot->removeSlot(id)) { slotMsg = "Slot removed. Total: " + to_string(lot->getTotalSlots()); slotIdInput.value = ""; } else slotMsg = "Cannot remove."; }
            catch (...) { slotMsg = "Invalid ID."; }
        }
        if (!slotMsg.empty()) { bool isErr = slotMsg.find("Cannot") != string::npos || slotMsg.find("Invalid") != string::npos; drawText(window, font, slotMsg, px + 300, py + 452, 12, isErr ? C_ERROR : C_SUCCESS, false, true); }
        Button saveBtn = { {px + 20, py + 470, 260, 40}, "Save to File", sf::Color(20,40,70), C_WHITE };
        Button exportBtn = { {px + 300, py + 470, 260, 40}, "Export Report", C_GOLD_DARK, C_WHITE };
        if (drawButton(window, font, saveBtn, lastEvent, clickThisFrame)) { FileHandler::saveSlots(lot); slotMsg = "Slots saved."; }
        if (drawButton(window, font, exportBtn, lastEvent, clickThisFrame)) { admin.exportReport(); slotMsg = "Report exported."; }
    }

    // ??? GENERATE REPORT ??????????????????????????????????????????
    void drawGenerateReport() {
        window.clear(C_BG); drawSidebar(Screen::GENERATE_REPORT); drawHeader(window, font, "Revenue Reports", 220, 1060);
        float px = 340, py = 78;
        drawRoundRect(window, px, py, 600, 560, 12.f, C_PANEL, C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ 600.f, 4.f }); topAcc.setPosition(px, py); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        drawText(window, font, "REVENUE REPORT", px + 300, py + 22, 16, C_GOLD, true, true);
        drawText(window, font, "Select period and generate report", px + 300, py + 46, 11, C_MUTED, false, true);
        drawDivider(window, px + 20, py + 66, 560);
        drawText(window, font, "REPORT PERIOD", px + 40, py + 82, 11, C_MUTED);
        string periodLabels[3] = { "Today", "This Week", "This Month" };
        float bw = 160, bh = 44, gap = 16;
        for (int i = 0; i < 3; i++) {
            float bx = px + 40 + i * (bw + gap);
            sf::FloatRect b(bx, py + 102, bw, bh);
            sf::Vector2f mouse;
            if (lastEvent.type == sf::Event::MouseMoved) mouse = { (float)lastEvent.mouseMove.x, (float)lastEvent.mouseMove.y };
            else if (lastEvent.type == sf::Event::MouseButtonPressed) mouse = { (float)lastEvent.mouseButton.x, (float)lastEvent.mouseButton.y };
            bool hov = b.contains(mouse), sel = (reportPeriod == i);
            sf::Color fill = sel ? C_GOLD_DARK : (hov ? sf::Color(25, 35, 70) : C_INPUT_BG);
            sf::Color border = sel ? C_GOLD : C_DIVIDER;
            drawRoundRect(window, bx, py + 102, bw, bh, 6.f, fill, border, sel ? 1.5f : 1.f);
            drawText(window, font, periodLabels[i], bx + bw / 2, py + 115, 13, sel ? C_GOLD : C_MUTED, sel, true);
            if (clickThisFrame && hov) { reportPeriod = i; reportGenerated = false; reportContent = ""; }
        }
        drawDivider(window, px + 20, py + 160, 560);
        Button genBtn = { {px + 20, py + 178, 260, 46}, "GENERATE REPORT", C_GOLD_DARK, C_WHITE };
        Button expBtn = { {px + 300, py + 178, 260, 46}, "EXPORT TO FILE", sf::Color(20,60,80), C_WHITE };
        if (drawButton(window, font, genBtn, lastEvent, clickThisFrame)) {
            string periodStr[3] = { "day", "week", "month" }; string p = periodStr[reportPeriod];
            ifstream file("data/history.txt"); int count = 0; float totalRevenue = 0; time_t now = time(nullptr); string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                stringstream ss(line); string tok; string parts[8]; int idx = 0;
                while (getline(ss, tok, ',') && idx < 8) parts[idx++] = tok;
                if (idx < 8) continue;
                time_t exitTime; try { exitTime = stol(parts[5]); }
                catch (...) { continue; }
                double diff = difftime(now, exitTime); bool include = false;
                if (p == "day" && diff <= 86400) include = true;
                if (p == "week" && diff <= 604800) include = true;
                if (p == "month" && diff <= 2592000) include = true;
                if (include) { count++; try { totalRevenue += stof(parts[7]); } catch (...) {} }
            }
            file.close();
            string periodNames[3] = { "Today", "This Week", "This Month" };
            reportContent = "Period     : " + periodNames[reportPeriod] + "\nRecords    : " + to_string(count) + "\nRevenue    : Rs " + to_string((int)totalRevenue);
            if (count > 0 && totalRevenue > 0) { float avg = totalRevenue / count; reportContent += "\nAverage    : Rs " + to_string((int)avg) + " per vehicle"; }
            reportGenerated = true;
        }
        if (drawButton(window, font, expBtn, lastEvent, clickThisFrame)) { admin.exportReport(); reportContent += "\n\nReport exported."; reportGenerated = true; }
        if (reportGenerated && !reportContent.empty()) {
            drawRoundRect(window, px + 20, py + 240, 560, 280, 8.f, sf::Color(10, 18, 35), C_GOLD_DARK, 1.f);
            sf::RectangleShape rh({ 560.f, 28.f }); rh.setPosition(px + 20, py + 240); rh.setFillColor(sf::Color(30, 22, 8)); window.draw(rh);
            drawText(window, font, "REPORT RESULT", px + 300, py + 246, 12, C_GOLD, true, true);
            stringstream ss(reportContent); string reportLine; float lineY = py + 282;
            while (getline(ss, reportLine)) {
                bool isRev = (reportLine.find("Revenue") != string::npos), isAvg = (reportLine.find("Average") != string::npos);
                drawText(window, font, reportLine, px + 40, lineY, 14, isRev ? C_GOLD : (isAvg ? C_SUCCESS : C_WHITE), isRev); lineY += 24;
            }
        }
        else if (!reportGenerated) drawText(window, font, "Select a period and click 'Generate Report'", px + 300, py + 320, 12, C_MUTED, false, true);
    }

    // ??? FIND VEHICLE ?????????????????????????????????????????????
    void drawFindVehicle() {
        window.clear(C_BG);
        bool isAdmin = admin.getLoginStatus();
        if (isAdmin) drawSidebar(Screen::FIND_VEHICLE);
        float cx = isAdmin ? 220.f : 0.f, cw = isAdmin ? 1060.f : 1280.f;
        if (isAdmin) drawHeader(window, font, "Find Vehicle", cx, cw);
        float panelX = cx + (cw - 700) / 2;
        drawRoundRect(window, panelX, 80, 700, 500, 12.f, C_PANEL, C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ 700.f, 4.f }); topAcc.setPosition(panelX, 80); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        drawText(window, font, "FIND MY VEHICLE", panelX + 350, 105, 18, C_GOLD, true, true);
        drawText(window, font, "Enter your vehicle number to locate it", panelX + 350, 130, 12, C_MUTED, false, true);
        drawDivider(window, panelX + 20, 152, 660);
        drawText(window, font, "VEHICLE NUMBER", panelX + 20, 168, 11, C_MUTED);
        findPlate.bounds = { panelX + 20, 186, 500, 44 }; drawInput(window, font, findPlate);
        Button searchBtn = { {panelX + 20, 250, 300, 46}, "SEARCH", C_GOLD_DARK, C_WHITE };
        Button clearBtn = { {panelX + 340, 250, 150, 46}, "Clear",  sf::Color(30,35,65), C_MUTED };
        Button backBtn = { {panelX + 510, 250, 170, 46}, "Back",   sf::Color(30,35,65), C_MUTED };
        if (drawButton(window, font, searchBtn, lastEvent, clickThisFrame)) {
            findResult = ""; findSuccess = false;
            if (findPlate.value.empty()) { findResult = "Please enter a vehicle number."; }
            else {
                ParkingSlot* found = lot->findVehicleByNumber(findPlate.value);
                if (found) {
                    Vehicle* v = found->getParkedVehicle();
                    findResult = "Vehicle FOUND!\n\nSlot Number : " + to_string(found->getSlotId()) + "\nSlot Type   : " + found->getType() + "\nVehicle No. : " + v->getVehicleNo() + "\nOwner       : " + v->getOwnerName() + "\nType        : " + v->getType() + "\nEntry Time  : " + timeToStr(v->getEntryTime()) + "\nEntry ID    : " + v->getEntryId();
                    findSuccess = true;
                }
                else findResult = "Vehicle not found.";
            }
        }
        if (drawButton(window, font, clearBtn, lastEvent, clickThisFrame)) { findPlate.value = ""; findResult = ""; findSuccess = false; }
        if (drawButton(window, font, backBtn, lastEvent, clickThisFrame)) currentScreen = isAdmin ? Screen::ADMIN_DASHBOARD : Screen::USER_DASHBOARD;
        if (!findResult.empty()) {
            sf::Color resultColor = findSuccess ? C_SUCCESS : C_ERROR;
            drawRoundRect(window, panelX + 20, 310, 660, findSuccess ? 200.f : 100.f, 8.f, sf::Color(findSuccess ? 15 : 40, findSuccess ? 40 : 15, findSuccess ? 25 : 15), resultColor, 1.f);
            float textY = 326; stringstream ss(findResult); string line;
            while (getline(ss, line)) { drawText(window, font, line, panelX + 350, textY, 13, (textY == 326 && findSuccess) ? C_GOLD : C_WHITE, (textY == 326 && findSuccess), true); textY += 22; }
        }
    }

    // ??? SLOT MAINTENANCE ?????????????????????????????????????????
    void drawSlotMaintenance() {
        window.clear(C_BG);
        drawSidebar(Screen::SLOT_MAINTENANCE);
        drawHeader(window, font, "Slot Maintenance", 220, 1060);

        float px = 340, py = 78;
        drawRoundRect(window, px, py, 600, 520, 12.f, C_PANEL, C_GOLD_DARK, 1.f);
        sf::RectangleShape topAcc({ 600.f, 4.f });
        topAcc.setPosition(px, py); topAcc.setFillColor(C_GOLD); window.draw(topAcc);
        drawText(window, font, "SLOT MAINTENANCE", px + 300, py + 22, 16, C_GOLD, true, true);
        drawText(window, font, "Mark slots as under maintenance or make them available",
            px + 300, py + 46, 11, C_MUTED, false, true);
        drawDivider(window, px + 20, py + 66, 560);

        // Current maintenance slots
        drawText(window, font, "SLOTS UNDER MAINTENANCE", px + 40, py + 82, 12, C_GOLD, true);
        int maintCount = 0;
        float listY = py + 106;
        for (int i = 0; i < lot->getTotalSlots(); i++) {
            ParkingSlot* s = lot->getSlot(i);
            if (s && s->getStatus() == "maintenance") {
                drawText(window, font, "Slot " + to_string(i) + " (" + s->getType() + ")",
                    px + 60, listY + maintCount * 22, 12, C_SLOT_MAINT);
                maintCount++;
            }
        }
        if (maintCount == 0)
            drawText(window, font, "No slots under maintenance.", px + 60, listY, 12, C_MUTED);

        drawDivider(window, px + 20, py + 340, 560);

        // Toggle section — HARDCODED POSITIONS
        float secY = py + 352;
        drawText(window, font, "TOGGLE SLOT STATUS", px + 40, secY, 12, C_GOLD, true);
        drawText(window, font, "Enter slot number (0-49) as shown on grid",
            px + 40, secY + 20, 11, C_MUTED);

        // Label and input on same line
        drawText(window, font, "Slot:", px + 60, secY + 50, 13, C_MUTED);
        maintenanceSlotId.bounds = { px + 110, secY + 42, 150, 38 };
        drawInput(window, font, maintenanceSlotId);

        // Buttons
        Button maintBtn = { {px + 60, secY + 95, 160, 42}, "MARK MAINT.", sf::Color(180, 140, 30), C_WHITE };
        Button availBtn = { {px + 240, secY + 95, 160, 42}, "MARK AVAIL.", C_SUCCESS, C_WHITE };

        if (drawButton(window, font, maintBtn, lastEvent, clickThisFrame)) {
            maintenanceMsg = "";
            if (maintenanceSlotId.value.empty()) {
                maintenanceMsg = "Enter a slot number first.";
            }
            else {
                try {
                    int idx = stoi(maintenanceSlotId.value);
                    ParkingSlot* s = lot->getSlot(idx);
                    if (!s) {
                        maintenanceMsg = "Slot " + to_string(idx) + " does not exist.";
                    }
                    else if (s->getStatus() == "occupied") {
                        maintenanceMsg = "Slot is occupied. Vacate it first.";
                    }
                    else if (s->getStatus() == "maintenance") {
                        maintenanceMsg = "Already under maintenance.";
                    }
                    else {
                        s->setStatus("maintenance");
                        maintenanceMsg = "Slot " + to_string(idx) + " ? MAINTENANCE";
                    }
                }
                catch (...) {
                    maintenanceMsg = "Invalid number.";
                }
            }
        }

        if (drawButton(window, font, availBtn, lastEvent, clickThisFrame)) {
            maintenanceMsg = "";
            if (maintenanceSlotId.value.empty()) {
                maintenanceMsg = "Enter a slot number first.";
            }
            else {
                try {
                    int idx = stoi(maintenanceSlotId.value);
                    ParkingSlot* s = lot->getSlot(idx);
                    if (!s) {
                        maintenanceMsg = "Slot " + to_string(idx) + " does not exist.";
                    }
                    else if (s->getStatus() != "maintenance") {
                        maintenanceMsg = "Slot is not under maintenance.";
                    }
                    else {
                        s->setStatus("available");
                        maintenanceMsg = "Slot " + to_string(idx) + " ? AVAILABLE";
                    }
                }
                catch (...) {
                    maintenanceMsg = "Invalid number.";
                }
            }
        }

        // Result message
        if (!maintenanceMsg.empty()) {
            bool isErr = (maintenanceMsg.find("does not exist") != string::npos ||
                maintenanceMsg.find("Invalid") != string::npos ||
                maintenanceMsg.find("occupied") != string::npos ||
                maintenanceMsg.find("not under") != string::npos ||
                maintenanceMsg.find("Already") != string::npos ||
                maintenanceMsg.find("Enter") != string::npos);
            drawText(window, font, maintenanceMsg, px + 60, secY + 152, 13,
                isErr ? C_ERROR : C_SUCCESS, false, false);
        }
    }
};

int main() {
    GrandValetApp app;
    app.run();
    return 0;
}