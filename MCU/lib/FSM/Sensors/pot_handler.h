/*
 * File: pot_handler.h
 * Description: Header for potentiometer height tracking functions.
 */

#ifndef POT_HANDLER_H
#define POT_HANDLER_H
#define HARDCODE 1

void initPot();
int getRawPotValue();
float getHeight();
void setZeroHeight();
void setMaxHeight();
void setMinHeight();
float getMaxHeightFeet();

#endif // POT_HANDLER_H
