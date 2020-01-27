//
// Created by User on 22/01/2020.
//

#ifndef TEXTANNOTATIONRECOGNITION_STEP2_POSITION_H
#define TEXTANNOTATIONRECOGNITION_STEP2_POSITION_H


class Position {
public:
    // Stores the Position components.
    unsigned int x, y;

    // Initializes a Position with a default (0,0) position.
    Position() : x(0), y(0) { }

    // Initializes a color Pixel with the specified RGB values.
    Position(int in_x, int in_y) : x(in_x), y(in_y){ }
};


#endif //TEXTANNOTATIONRECOGNITION_STEP2_POSITION_H
