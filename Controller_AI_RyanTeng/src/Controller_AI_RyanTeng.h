// MIT License
// 
// Copyright(c) 2020 Arthur Bacon and Kevin Dill
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "iController.h"


enum Difficulty { very_easy, easy, medium, hard, extreme };

enum Personality { random, defensive, aggressive };

class Controller_AI_RyanTeng : public iController
{
public:
    Controller_AI_RyanTeng();
    Controller_AI_RyanTeng(Difficulty difficulty, Personality personality);

    virtual ~Controller_AI_RyanTeng() {}

    void tick(float deltaTSec);

private:
    float diff = 0.75;
    Personality personality = random;
    float west_lane_pressure = 0;
    float east_lane_pressure = 0;
    float diff_pressure_west = 0;
    float diff_pressure_east = 0;

    void calculatePressure();
};