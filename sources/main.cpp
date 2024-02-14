/*
 * Project   Machine Controller Software
 * Author    Jean-François Simon
 * Company   Chrysalide Engineering
 * Date      2024/02/14
 * Version   1.0
 */

/*
 *  Copyright 2024 Jean‐François Simon, Chrysalide Engineering
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright  notice,  this
 * list of conditions and the following disclaimer.
 *
 * 2.  Redistributions  in  binary  form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3.  Neither  the  name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from  this  software  without
 * specific prior written permission.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  TO,  THE  IMPLIED
 * WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAI‐
 * MED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  LIABLE  FOR  ANY
 * DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (IN‐
 * CLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  SERVICES;  LOSS
 * OF  USE,  DATA,  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR  TORT  (INCLUDING
 * NEGLIGENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * TODO
 *
 * - GCode Parser
 * - Windows resize
 * - Windows build
 * - State machine
 * - LCD animation (startup screen, test, date/time and status, state ...)
 * - Reset PB (do not lock, use states)
 * - 7-segment display demo (8.)
 * - Complete Classes (7-segment digits and display) and clean into CSL header and cpp
 * - Panelize (panel class to hold position for widgets)
 * - Generalyze serial com
 * - Keep files in sync
 * - Migrate speed ramps (accelerations) into firmware
 *
 */

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "csl.h"

using namespace std;

void ramp_up (unsigned int, unsigned int);
void ramp_down (unsigned int, unsigned int);

// Serial
#define MOT_CMD_RUN       1
#define MOT_CMD_SLEEP     2
#define MOT_CMD_PAUSE     3
#define MOT_CMD_MODE_AUTO 4
#define MOT_CMD_MODE_MAN  5
#define MOT_CMD_SPD_PLUS  6
#define MOT_CMD_SPD_MINUS 7
#define MOT_CMD_DIR_CCW   8
#define MOT_CMD_DIR_CW    9
#define MOT_CMD_GO_SLOW   10
#define MOT_CMD_HOLD_POS  11

csl::Serial Serial;

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(908, 468), "CNC Gui");

    // Load a sprite to display

    // Instanciate buttons
    csl::Push_button reset_pb {"medias/bitmap25.png", "medias/bitmap26.png", sf::Vector2<float>(780,350), 0.50};
    csl::Push_button test_pb {"medias/bitmap27.png", "medias/bitmap28.png", sf::Vector2<float>(690,350), 0.50};

    // Migration New PBs
    const float pb_scale {0.55};
    const int panel_posx {8}, panel_posy {190};
    // Load pushbuttons (texture when on, texture when off, position, scale)
    csl::Push_button off_pb {"medias/bitmap1.png", "medias/bitmap2.png", sf::Vector2<float>(50 + panel_posx, 50 + panel_posy), pb_scale};
    csl::Push_button on_pb {"medias/bitmap3.png", "medias/bitmap4.png", sf::Vector2<float>(150 + panel_posx, 50 + panel_posy), pb_scale};
    csl::Push_button ccw_pb {"medias/bitmap5.png", "medias/bitmap6.png", sf::Vector2<float>(250 + panel_posx, 50 + panel_posy), pb_scale};
    csl::Push_button pause_pb {"medias/bitmap22.png", "medias/bitmap23.png", sf::Vector2<float>(350 + panel_posx, 50 + panel_posy), pb_scale};
    csl::Push_button cw_pb {"medias/bitmap7.png", "medias/bitmap8.png", sf::Vector2<float>(450 + panel_posx, 50 + panel_posy), pb_scale};
    csl::Push_button m_pb {"medias/bitmap9.png", "medias/bitmap10.png", sf::Vector2<float>(50 + panel_posx, 150 + panel_posy), pb_scale};
    csl::Push_button a_pb {"medias/bitmap11.png", "medias/bitmap12.png", sf::Vector2<float>(150 + panel_posx, 150 + panel_posy), pb_scale};
    csl::Push_button minus_pb {"medias/bitmap19.png", "medias/bitmap20.png", sf::Vector2<float>(300 + panel_posx, 150 + panel_posy), pb_scale};
    csl::Push_button plus_pb {"medias/bitmap17.png", "medias/bitmap18.png", sf::Vector2<float>(400 + panel_posx, 150 + panel_posy), pb_scale};

    // Vector PB
    vector<csl::Push_button*> pb_v;
    pb_v.push_back(&reset_pb);
    pb_v.push_back(&test_pb);
    pb_v.push_back(&minus_pb);
    pb_v.push_back(&plus_pb);
    pb_v.push_back(&a_pb);
    pb_v.push_back(&m_pb);
    pb_v.push_back(&cw_pb);
    pb_v.push_back(&pause_pb);
    pb_v.push_back(&ccw_pb);
    pb_v.push_back(&on_pb);
    pb_v.push_back(&off_pb);

    // Background
    sf::Texture tex_bg;
    if (!tex_bg.loadFromFile("medias/bg.png")) return EXIT_FAILURE;
    sf::Sprite sprite_bg(tex_bg);
    sprite_bg.setScale(0.7, 0.7);

    // Load Font
    sf::Text lcd_display;
    sf::Font font_lcd_display;
    {
        // 7 Seg Font

        if (!font_lcd_display.loadFromFile("fonts/7segment.ttf")) {
            cout << "Error loading font" << endl;
            return EXIT_FAILURE;
        }
        lcd_display.setFont(font_lcd_display);
        lcd_display.setCharacterSize(50); // Size pixels
        lcd_display.setFillColor(sf::Color::White);
        lcd_display.setPosition(75,106);

        lcd_display.setString("GOOD NORNING LCD 240\n1234567890 ABCDEFGHI");
    }

    // 7 Segment displays
    csl::Seven_seg_display cyc {5}, spd {5}, pos {5}, cur {5};
    {
        const sf::Vector2<int> ref_pos {713 + 4*30, 90}; // Pos 713 from left dig
                                                         // while 7-seg digits class
                                                         // pos ref on right most digit
        const int v_spacing {64};
        const float scale {0.55};
        cyc.set_position(ref_pos.x, ref_pos.y + 0 * v_spacing);
        spd.set_position(ref_pos.x, ref_pos.y + 1 * v_spacing);
        pos.set_position(ref_pos.x, ref_pos.y + 2 * v_spacing);
        cur.set_position(ref_pos.x, ref_pos.y + 3 * v_spacing);
        cyc.set_scale(scale);
        spd.set_scale(scale);
        pos.set_scale(scale);
        cur.set_scale(scale);
    }

  window.setFramerateLimit(60);

  // Init
  Serial.non_blocking_write(MOT_CMD_SLEEP);
  Serial.non_blocking_write(MOT_CMD_MODE_MAN);
  Serial.non_blocking_write(MOT_CMD_DIR_CW);

  // ************* Main loop *************

	// Start the game loop
    while (window.isOpen())
    {
        // 7 segment display demo
        /*
        static int cyc_cnt {0}, spd_cnt {0}, pos_cnt {0}, cur_cnt {0};
        cyc = cyc_cnt++;
        spd = spd_cnt++;
        pos = pos_cnt++;
        cur = cur_cnt++;
        */

        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                window.close();
            // Mouse click
            {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        sf::Vector2<int> mp = sf::Mouse::getPosition(window);
                        if (on_pb.is_pressed(window)) {
                            cout << "Click Sprite P1 ON" << endl;
                            Serial.non_blocking_write(MOT_CMD_HOLD_POS);
                            // Serial.non_blocking_write(MOT_CMD_RUN);
                            // Push buttons illumination
                            {
                                a_pb.set_off();
                                m_pb.set_on();
                                on_pb.set_on();
                                off_pb.set_off();
                                ccw_pb.set_off();
                                pause_pb.set_off();
                                cw_pb.set_off();
                            }
                        } else if (off_pb.is_pressed(window)) {
                            cout << "Click Sprite P2 OFF" << endl;
                            Serial.non_blocking_write(MOT_CMD_SLEEP);
                            // Push buttons illumination
                            {
                                a_pb.set_off();
                                m_pb.set_on();
                                on_pb.set_off();
                                off_pb.set_on();
                            }
                        } else if (pause_pb.is_pressed(window)) {
                            cout << "Click Sprite P3 Pause" << endl;
                            Serial.non_blocking_write(MOT_CMD_PAUSE);
                            // Push buttons illumination
                            {
                                ccw_pb.set_off();
                                pause_pb.set_on();
                                cw_pb.set_off();
                            }
                        } else if (a_pb.is_pressed(window)) {
                            cout << "Click Sprite P4 Auto" << endl;
                            Serial.non_blocking_write(MOT_CMD_MODE_AUTO);
                            // Push buttons illumination
                            {
                                a_pb.set_on();
                                m_pb.set_off();
                            }
                        } else if (m_pb.is_pressed(window)) {
                            cout << "Click Sprite P5 Man" << endl;
                            Serial.non_blocking_write(MOT_CMD_MODE_MAN);
                            // Push buttons illumination
                            {
                                a_pb.set_off();
                                m_pb.set_on();
                            }
                        } else if (plus_pb.is_pressed(window)) {
                            cout << "Click Sprite P6 Spd +" << endl;
                            // Push buttons illumination
                            {
                                plus_pb.set_on();
                                window.draw(plus_pb);
                              window.display();
                            }
                            ramp_up(5, 15);
                            // Push buttons illumination
                            {
                                plus_pb.set_off();
                                window.draw(plus_pb);
                              window.display();
                            }
                        } else if (minus_pb.is_pressed(window)) {
                            cout << "Click Sprite P7 Spd -" << endl;
                            // Push buttons illumination
                            {
                                minus_pb.set_on();
                                window.draw(minus_pb);
                                window.display();
                            }
                            ramp_down(5, 15);
                            // Push buttons illumination
                            {
                                minus_pb.set_off();
                                window.draw(minus_pb);
                                window.display();
                            }
                        } else if (ccw_pb.is_pressed(window)) {
                            cout << "Click Sprite P8 Dir CCW" << endl;
                            Serial.non_blocking_write(MOT_CMD_DIR_CCW);
                            // Push buttons illumination
                            {
                                ccw_pb.set_on();
                                pause_pb.set_off();
                                cw_pb.set_off();
                            }
                        } else if (cw_pb.is_pressed(window)) {
                            cout << "Click Sprite P9 Dir CW" << endl;
                            Serial.non_blocking_write(MOT_CMD_DIR_CW);
                            // Push buttons illumination
                            {
                                ccw_pb.set_off();
                                pause_pb.set_off();
                                cw_pb.set_on();
                            }
                        } else if (test_pb.is_pressed(window)) {
                            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                                for (auto i:pb_v) {
                                    i->set_test();
                                    window.draw(*i);
                                }
                                window.display();
                            }
                            for (auto i:pb_v) i->set_normal();
                        }
                    }
                } else if (event.type == sf::Event::KeyPressed) {
                    auto kp {event.key.code};
                    cout << kp << endl;
                    if (kp == 15) {
                        if (event.key.control) cout << "Ctrl P" << endl;
                        else if (event.key.alt) cout << "Alt P" << endl;
                        else if (event.key.shift) cout << "Shift P" << endl;
                        else cout << "P" << endl;
                    }
                }
            }
        }

        // Load text
        static string str;
        str = Serial.blocking_read();
        if (str.size()) {
          // cout << str << flush;
          // Cases (read backs from control board)
          if (str.find('.') != string::npos) {
                static unsigned int cycle_count {0};
                cycle_count++;
                cyc = cycle_count;
          } else if (str.find(">=") != string::npos) {
            int stepper_position {0};

            std::string s {str};
            std::string delimiter = ">=";

            size_t pos_l = 0;
            std::string token;
            while ((pos_l = s.find(delimiter)) != std::string::npos) {
                token = s.substr(0, pos_l);
                // std::cout << "<" << token << ">" << std::endl;
                if (token.size()) {
                int token_sane {1};
                for (auto i:token) {
                  if ((i != '-') && (i != '0') && (i != '1') && (i != '2') && \
                      (i != '3') && (i != '4') && (i != '5') && (i != '6') && \
                      (i != '7') && (i != '8') && (i != '9')) token_sane = 0;
                  }
                  if (token_sane) {
                    stepper_position = stoi(token);
                    pos = stepper_position / 4; // Unit / microsteps conversion
                  }
                }
                s.erase(0, pos_l + delimiter.length());
            }
          }
          str = "";
        }

        // Clear screen
        window.clear(sf::Color(219,226,227,255));

        // Draw the sprite
        window.draw(sprite_bg);

        // Draw PBs
        for (auto i:pb_v) window.draw(*i);

        window.draw(lcd_display);

        cyc.draw(&window);
        spd.draw(&window);
        pos.draw(&window);
        cur.draw(&window);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}

void ramp_up (unsigned int steps, unsigned int dt_ms) {
  sf::Clock clock;
  sf::Time dt {sf::milliseconds (dt_ms)};

  for (int i {0}; i < steps; i++) {
    // Delay
    while (clock.getElapsedTime() < dt);
    clock.restart();
    Serial.non_blocking_write(MOT_CMD_SPD_PLUS);
  }

}
void ramp_down (unsigned int steps, unsigned int dt_ms) {
  sf::Clock clock;
  sf::Time dt {sf::milliseconds (dt_ms)};

  for (int i {0}; i < steps; i++) {
    // Delay
    while (clock.getElapsedTime() < dt);
    clock.restart();
    Serial.non_blocking_write(MOT_CMD_SPD_MINUS);
  }
}
