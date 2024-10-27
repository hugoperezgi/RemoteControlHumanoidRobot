#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

//Std Max-Min of servos
#define SERVOMIN  71 // This is the 'minimum' pulse length count (out of 4096) AKA 0
#define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096) AKA 180º

//yellow color on servo // White marking on cable
#define SERVOMIN_Rotation_Biceps_y  71 
#define SERVOMAX_Rotation_Biceps_y  410 
#define SERVOMIN_Codo_y  220 
#define SERVOMAX_Codo_y  455 
#define SERVOMIN_Rotation_Antebrazo_y  71 
#define SERVOMAX_Rotation_Antebrazo_y  400 
//yellow color on servo // White marking on cable

//The other arm
#define SERVOMIN_Rotation_Biceps  90 
#define SERVOMAX_Rotation_Biceps  400 
#define SERVOMIN_Codo  100 
#define SERVOMAX_Codo  330 
#define SERVOMIN_Rotation_Antebrazo  90 
#define SERVOMAX_Rotation_Antebrazo  435 
//The other arm

//Rotation
#define SERVOMIDDLE_Rotation_Head 263 
#define SERVOMAX_Rotation_Head 349 
#define SERVOMIN_Rotation_Head 167
//Tilt With marker
#define SERVOMIDDLE_tiltHead_y 250
#define SERVOMIN_tiltHead_y 75
#define SERVOMAX_tiltHead_y 445
//Tilt NO marker
#define SERVOMIDDLE_tiltHead 260
#define SERVOMIN_tiltHead 75
#define SERVOMAX_tiltHead 455
//Head movements

//Shoulder movements
//Not marked side
#define SERVOMAX_shoulderarm 455 //Resting position, parallel with body
#define SERVOMIN_shoulderarm 140 //perpendicular with body
#define SERVOMAX_shoulderbody 455 //Upwards motion of the arm
#define SERVOMIN_shoulderbody 260 //Resting position, parallel with body - ish
//marked side
#define SERVOMAX_shoulderarm_y 200 //Resting position, parallel with body
#define SERVOMIN_shoulderarm_y 440 //perpendicular with body
#define SERVOMAX_shoulderbody_y 455 //Resting position, parallel with body - ish
#define SERVOMIN_shoulderbody_y 320 //Upwards motion of the arm
//Shoulder movements

//Right hand
#define SERVOMIN_pinky_R 130 //green
#define SERVOMAX_pinky_R 370
#define SERVOMIN_ring_R 150 //blue
#define SERVOMAX_ring_R 400
#define SERVOMIN_middle_R 150 //yellow
#define SERVOMAX_middle_R 420 //dedo estirado
#define SERVOMIN_index_R 150 //red
#define SERVOMAX_index_R 400
#define SERVOMIN_thumb_R 0
#define SERVOMAX_thumb_R 0
#define SERVOMIN_palm_R 230 //gray
#define SERVOMAX_palm_R 450
//Left Hand
#define SERVOMIN_pinky_L 170
#define SERVOMAX_pinky_L 370
#define SERVOMIN_ring_L 170 //blue
#define SERVOMAX_ring_L 370
#define SERVOMIN_middle_L 170 //Yellow
#define SERVOMAX_middle_L 370
#define SERVOMIN_index_L 170//red
#define SERVOMAX_index_L 370
#define SERVOMIN_thumb_L 200
#define SERVOMAX_thumb_L 450
#define SERVOMIN_palm_L 280 //mano cerrada
#define SERVOMAX_palm_L 420

//MAX es dedo estirado, min es puño cerrado

//Torso
#define SERVOMIN_torso_y 90 //Max rotation up
#define SERVOMAX_torso_y 450 //Max rotation down
#define SERVOMIDDLE_torso_y 290 //Hold position-Middle position

#define SERVOMIN_torso 90 //Max rotation down
#define SERVOMAX_torso 450 //Max rotation up
#define SERVOMIDDLE_torso 280 //Hold position-Middle position



//Board1 - Left Hand(6) // Board3 - Right Hand(6)
    //Board1/3 ids
    // thumb 0, index 1, middle 2, ring 3, pinky 4, palm 5

//Board2 - Arms/Torso/Head(3x2/6/3)
    //Board2 ids
    // forearmRotationL - 0 / armFlexL - 2 / bicepsRotationL - 4 / shoulderArmRotationL - 6 / shoulderTorsoRotationL - 8
    // forearmRotationR - 1 / armFlexR - 3 / bicepsRotationR - 5 / shoulderArmRotationR - 7 / shoulderTorsoRotationR - 9
    // torsoTiltL - 10 / torsoTiltR - 11
    // headTiltL - 13 / headTiltR - 14 / headRotation - 15