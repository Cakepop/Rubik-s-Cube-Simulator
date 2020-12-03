/* Lucas Samaniego
   Rubik's cube
   11/27/2020
   Purpose: models an interchangeable Rubik's cube in Arduino C
*/

#include <stdio.h>
#include <string.h>
#define SIZE 9  //number of stickers per side
#define SCRAMBLE_FACTOR 100 //number of turns in an RNG scramble

//global variables
int state = 0;
int top[SIZE];
int left[SIZE];
int front[SIZE];
int right[SIZE];
int back[SIZE];
int bottom[SIZE];
int user_input;
char F, R, U, L, B, D;
char extra_input;

//High-level functions (Start/Menu)
void initialize();
void print_menu();
void menu_interact(); //Sub-menu interaction

//Middle-level, standalone functions that are used inside and outside of other functions
void print_example_cube();
void print_cube_colors();
void print_inputs();
void sandbox();
void scramble();
void apply_inputs(int user_input);
String svtc(int sticker_value); //alias for convert-sticker-value-to-color function
String convert_sticker_value_to_color(int sticker_value);

//Low-level function-prototypes for 90° turns of a layer; used to make all turns on a cube
void Front();
void Right();
void Up();
void Left();
void Back();
void Down();

//Debugging functions only used during back-end development
void print_cube_values(); //prints 2D plot with values of each square of cube
void print_raw(); //prints 0-53 number grid for each square of cube
void print_array(int side[], int array_length); //prints a single array as a row of values e.g. (in print_raw();)
String svtc_reverse(int sticker_value); //sticker-color-to-value

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println("Rubik's Cube");
  Serial.println("by Lucas Samaniego\n");

  Serial.println("Initializing...");
  initialize();

  Serial.print("\n\n");

  Serial.println("Note: The cube will always be displayed as a 2D drawing with the following orientation:\n");
  print_example_cube();

  print_menu();
  menu_interact();

  while (extra_input != 'M') {
    cube_interact();
  }
  print_menu();
  menu_interact();

  Serial.println("End program.");
}

void loop() {
}

//
void cube_interact() {
  Serial.print("Enter a command(or M to return to menu): ");
  while (Serial.available() == 0);
  char extra_input = Serial.read();
  Serial.println(extra_input);
  if (extra_input == 'M') {
    print_menu();
    menu_interact();
  }
  else {
    apply_inputs(extra_input);
  }

}

//initialize cube array from solved position
void initialize() {
  for (int index = 0; index < 54; index++) {
    if (index < 9) {                              // 0-8
      top[index] = index;
    }
    if (index >= SIZE && index < 2 * SIZE) {      // 9-17
      left[index - SIZE] = index;
    }
    if (index >= 2 * SIZE && index < 3 * SIZE) {  // 18-26
      front[index - 2 * SIZE] = index;
    }
    if (index >= 3 * SIZE && index < 4 * SIZE) {  // 27-36
      right[index - 3 * SIZE] = index;
    }
    if (index >= 4 * SIZE && index < 5 * SIZE) {  // 37-46
      back[index - 4 * SIZE] = index;
    }
    if (index >= 5 * SIZE && index < 6 * SIZE) {  // 46-53
      bottom[index - 5 * SIZE] = index;
    }
  }
}

//Print Menu Options
void print_menu() {
  Serial.println("Menu Options:");
  Serial.println("Sandbox: Freely turn a solved cube: 1");
  Serial.println("Scramble: Start with a randomnly mixed cube: 2");
  Serial.println("End program: 3\n");
  //add future features to the menu here
}

//prints an example of how to perceive the cube orientation at all times with the 2d unfolded cross-pattern layout
void print_example_cube() {
  Serial.println("\t   +--------+\t\t        _ _ _");
  Serial.println("\t  /        /|\t\t       |      |");
  Serial.println("\t /  TOP   / |\t\t       |  T   |");
  Serial.println("\t+--------+  |\t\t _ _ _ |_ _ _ | _ _ _  _ _ _");
  Serial.println("\t|        | Right\t|      |      |       |      |");
  Serial.println("\t| FRONT  |  +\t\t|  L   |  F   |   R   |   B  |");
  Serial.println("\t|        | /\t\t|_ _ _ | _ _ _| _ _ _ |_ _ _ |");
  Serial.println("\t|        |/\t\t       |      |");
  Serial.println("\t+--------+\t\t       |  U   |");
  Serial.println("\t\t\t\t       | _ _ _|\n");              //cube ascii visualization taken from: https://codegolf.stackexchange.com/questions/189/drawing-a-cube-in-ascii-art
}

//allows user to interact with menu and change modes
void menu_interact() {
  while (state < 1 || state > 3) {
    Serial.print("Enter a valid option: ");
    while (Serial.available() == 0);
    state = Serial.parseInt();
    Serial.println(state);

    //interact with cube from solved state
    if (state == 1) {
      sandbox();
    }
    //interact with cube from scrambled state
    if (state == 2) {
      scramble();
      print_cube_colors();
    }
    if (state == 3) {
      Serial.println("End of program.");
    }
    else {
    }
  }

}

//prints the cube with colors as designated by their value stored in the array for each side
void print_cube_colors() {
  /*0        _ _ _
    1       |      |
    2       |      |
    3 _ _ _ |_ _ _ | _ _ _  _ _ _
    4|      |      |       |      |
    5|      |      |       |      |
    6|_ _ _ | _ _ _| _ _ _ |_ _ _ |
    7       |      |
    8       |      |
    9       | _ _ _|
                              cross-shape of 6-sided cube unfolded as a 2D-plot of sticker colors
  */
  String pie = "" ; //pie is a placeholder string variable to make concatenation possible
  Serial.println("               __  __  __");
  Serial.println(pie + "             |" + svtc(top[0]) + " " + svtc(top[1]) + " " + svtc(top[2]) + "|");
  Serial.println(pie + "             |" + svtc(top[3]) + " " + svtc(top[4]) + " " + svtc(top[5]) + "|");
  Serial.println(pie + "  __  __  __ |" + svtc(top[6]) + " " + svtc(top[7]) + " " + svtc(top[8]) + "| __  __  __  __  __  __");
  Serial.println(pie + " |" + svtc(left[0]) + " " + svtc(left[1]) + " " + svtc(left[2]) + "|" + svtc(front[0]) + " " + svtc(front[1]) + " " + svtc(front[2]) + "|" + svtc(right[0]) + " " + svtc(right[1]) + " " + svtc(right[2]) + "|" + svtc(back[0]) + " " + svtc(back[1]) + " " + svtc(back[2]) + "|");
  Serial.println(pie + " |" + svtc(left[3]) + " " + svtc(left[4]) + " " + svtc(left[5]) + "|" + svtc(front[3]) + " " + svtc(front[4]) + " " + svtc(front[5]) + "|" + svtc(right[3]) + " " + svtc(right[4]) + " " + svtc(right[5]) + "|" + svtc(back[3]) + " " + svtc(back[4]) + " " + svtc(back[5]) + "|");
  Serial.println(pie + " |" + svtc(left[6]) + " " + svtc(left[7]) + " " + svtc(left[8]) + "|" + svtc(front[6]) + " " + svtc(front[7]) + " " + svtc(front[8]) + "|" + svtc(right[6]) + " " + svtc(right[7]) + " " + svtc(right[8]) + "|" + svtc(back[6]) + " " + svtc(back[7]) + " " + svtc(back[8]) + "|");
  Serial.println(pie + "             |" + svtc(bottom[0]) + " " + svtc(bottom[1]) + " " + svtc(bottom[2]) + "|");
  Serial.println(pie + "             |" + svtc(bottom[3]) + " " + svtc(bottom[4]) + " " + svtc(bottom[5]) + "|");
  Serial.println(pie + "             |" + svtc(bottom[6]) + " " + svtc(bottom[7]) + " " + svtc(bottom[8]) + "|");
}

//prints options for inputs to manipulate the cube
void print_inputs() {
  Serial.print("Enter any of the following commands:\n");
  Serial.println("\tF - Rotates the front layer clockwise 90°");
  Serial.println("\tR - Rotates the right layer clockwise 90°");
  Serial.println("\tU - Rotates the top layer clockwise 90°");
  Serial.println("\tL - Rotates the left layer clockwise 90°");
  Serial.println("\tB - Rotates the back layer clockwise 90°");
  Serial.println("\tD - Rotates the bottom layer clockwise 90°");

  Serial.println("\tM - Exit and return to Menu"); Serial.print("\n");
}

void sandbox() {
  print_cube_colors();
  Serial.print("\n");
  print_inputs();
  Serial.print("Entered input: ");
  while (Serial.available() == 0);
  int user_input = Serial.read();
  char char_version_of_user_input = user_input;
  Serial.println(char_version_of_user_input);
  apply_inputs(user_input);
}

//applies 25 random turns to the cube to randomize the colors
void scramble() {
  for ( int cakes = 0; cakes <= SCRAMBLE_FACTOR; cakes++) { //variable cakes is simply a counter
    int turn = random(1, 6);
    switch (turn) {
      case 1:
        Front();
        break;
      case 2:
        Right();
        break;
      case 3:
        Up();
        break;
      case 4:
        Left();
        break;
      case 5:
        Back();
        break;
      case 6:
        Down();
    }
  }
}

//converts user inputs into functional code
void apply_inputs(int user_input) {
  switch (user_input) {
    case 'F':
      Front();
      break;
    case 'R':
      Right();
      break;
    case 'U':
      Up();
      break;
    case 'L':
      Left();
      break;
    case 'B':
      Back();
      break;
    case 'D':
      Down();
      break;
    case 'M':
      Serial.println("\nReturning to Menu...\n");
      print_menu();
      menu_interact();
  }
  print_cube_colors();
}

//alias of convert_sticker_value_to_color for simplicity
String svtc(int sticker_value) {
  return (convert_sticker_value_to_color(sticker_value));
}

// converts stickers xx through xx to print color on the display as "COL" 00-53
String convert_sticker_value_to_color(int sticker_value) {
  if (sticker_value >= 0 && sticker_value < SIZE ) {                  //00-08 yellow
    return ("YEL");
  }
  if (sticker_value >= SIZE && sticker_value < 2 * SIZE ) {           //09-17 orange
    return ("ORG");
  }
  if (sticker_value >= 2 * SIZE && sticker_value < 3 * SIZE ) {       //18-26 blue
    return ("BLU");
  }
  if (sticker_value >= 3 * SIZE && sticker_value < 4 * SIZE) {       //27-35 red
    return ("RED");
  }
  if (sticker_value >= 4 * SIZE && sticker_value < 5 * SIZE) {        //36-44 green
    return ("GRN");
  }
  if (sticker_value >= 5 * SIZE && sticker_value < 6 * SIZE) {        //45-53 white
    return ("WHT");
  }
  else {                                                            //error case handle
    return ("ERR");
  }
}

// These 6 functions; Front(), Right(), Up(), Left(), Back(), Down(), are used to make any permutations to the cube
//********************************************************Front Turn*************************************************
void Front() {
  int front_a = left[2];
  int front_b = left[5];
  int front_c = left[8];

  left[2] = bottom[0];
  left[5] = bottom[1];
  left[8] = bottom[2];

  bottom[0] = right[6];
  bottom[1] = right[3];
  bottom[2] = right[0];

  right[0] = top[6];
  right[3] = top[7];
  right[6] = top[8];

  top[6] = front_c;
  top[7] = front_b;
  top[8] = front_a;

  int front_y = front[0];
  int front_z = front[1];
  front[0] = front[6];
  front[6] = front[8];
  front[8] = front[2];
  front[2] = front_y;
  front[1] = front[3];
  front[3] = front[7];
  front[7] = front[5];
  front[5] = front_z;
}

//*******************************************************Right Turn**************************************************
void Right() {
  int right_a = front[2];
  int right_b = front[5];
  int right_c = front[8];

  front[2] = bottom[2];
  front[5] = bottom[5];
  front[8] = bottom[8];

  bottom[2] = back[6];
  bottom[5] = back[3];
  bottom[8] = back[0];

  back[0] = top[8];
  back[3] = top[5];
  back[6] = top[2];

  top[2] = right_c;
  top[5] = right_b;
  top[8] = right_a;

  int right_y = right[0];
  int right_z = right[1];
  right[0] = right[6];
  right[6] = right[8];
  right[8] = right[2];
  right[2] = right_y;
  right[1] = right[3];
  right[3] = right[7];
  right[7] = right[5];
  right[5] = right_z;
}

//*******************************************************Up Turn**************************************************
void Up() {
  int up_a = front[0];
  int up_b = front[1];
  int up_c = front[2];

  front[0] = right[0];
  front[1] = right[1];
  front[2] = right[2];

  right[0] = back[0];
  right[1] = back[1];
  right[2] = back[2];

  back[0] = left[0];
  back[1] = left[1];
  back[2] = left[2];

  left[0] = up_a;
  left[1] = up_b;
  left[2] = up_c;

  int up_y = top[0];
  int up_z = top[1];
  top[0] = top[6];
  top[6] = top[8];
  top[8] = top[2];
  top[2] = up_y;
  top[1] = top[3];
  top[3] = top[7];
  top[7] = top[5];
  top[5] = up_z;
}

//*******************************************************Left Turn**************************************************
void Left() {
  int left_a = front[0];
  int left_b = front[3];
  int left_c = front[6];

  front[0] = top[0];
  front[3] = top[3];
  front[6] = top[6];

  top[0] = back[8];
  top[3] = back[5];
  top[6] = back[2];

  back[8] = bottom[0];
  back[5] = bottom[3];
  back[2] = bottom[6];

  bottom[0] = left_a;
  bottom[3] = left_b;
  bottom[6] = left_c;

  int left_y = left[0];
  int left_z = left[1];
  left[0] = left[6];
  left[6] = left[8];
  left[8] = left[2];
  left[2] = left_y;
  left[1] = left[3];
  left[3] = left[7];
  left[7] = left[5];
  left[5] = left_z;
}

//*******************************************************Back Turn**************************************************
void Back() {
  int back_a = right[2];
  int back_b = right[5];
  int back_c = right[8];

  right[2] = bottom[8];
  right[5] = bottom[7];
  right[8] = bottom[6];

  bottom[8] = left[0];
  bottom[7] = left[3];
  bottom[6] = left[6];

  left[0] = top[2];
  left[3] = top[1];
  left[6] = top[0];

  top[0] = back_a;
  top[1] = back_b;
  top[2] = back_c;

  int back_y = back[0];
  int back_z = back[1];
  back[0] = back[6];
  back[6] = back[8];
  back[8] = back[2];
  back[2] = back_y;
  back[1] = back[3];
  back[3] = back[7];
  back[7] = back[5];
  back[5] = back_z;
}

//*******************************************************Down Turn**************************************************
void Down() {
  int down_a = front[6];
  int down_b = front[7];
  int down_c = front[8];

  front[6] = left[6];
  front[7] = left[7];
  front[8] = left[8];

  left[6] = back[6];
  left[7] = back[7];
  left[8] = back[8];

  back[6] = right[6];
  back[7] = right[7];
  back[8] = right[8];

  right[6] = down_a;
  right[7] = down_b;
  right[8] = down_c;

  int down_y = bottom[0];
  int down_z = bottom[1];
  bottom[0] = bottom[6];
  bottom[6] = bottom[8];
  bottom[8] = bottom[2];
  bottom[2] = down_y;
  bottom[1] = bottom[3];
  bottom[3] = bottom[7];
  bottom[7] = bottom[5];
  bottom[5] = down_z;
}

//Debugging Functions

//prints values stored in each address for each array as a visual 2D plot (unfolded cube in cross shape)
void print_cube_values() {
  String pie = "" ; //pie is a placeholder string variable to make concatenation possible
  Serial.println("               __  __  __");
  Serial.println(pie + "             |" + svtc_reverse(top[0]) + " " + svtc_reverse(top[1]) + " " + svtc_reverse(top[2]) + "|");
  Serial.println(pie + "             |" + svtc_reverse(top[3]) + " " + svtc_reverse(top[4]) + " " + svtc_reverse(top[5]) + "|");
  Serial.println(pie + "  __  __  __ |" + svtc_reverse(top[6]) + " " + svtc_reverse(top[7]) + " " + svtc_reverse(top[8]) + "| __  __  __  __  __  __");
  Serial.println(pie + " |" + svtc_reverse(left[0]) + " " + svtc_reverse(left[1]) + " " + svtc_reverse(left[2]) + "|" + svtc_reverse(front[0]) + " " + svtc_reverse(front[1]) + " " + svtc_reverse(front[2]) + "|" + svtc_reverse(right[0]) + " " + svtc_reverse(right[1]) + " " + svtc_reverse(right[2]) + "|" + svtc_reverse(back[0]) + " " + svtc_reverse(back[1]) + " " + svtc_reverse(back[2]) + "|");
  Serial.println(pie + " |" + svtc_reverse(left[3]) + " " + svtc_reverse(left[4]) + " " + svtc_reverse(left[5]) + "|" + svtc_reverse(front[3]) + " " + svtc_reverse(front[4]) + " " + svtc_reverse(front[5]) + "|" + svtc_reverse(right[3]) + " " + svtc_reverse(right[4]) + " " + svtc_reverse(right[5]) + "|" + svtc_reverse(back[3]) + " " + svtc_reverse(back[4]) + " " + svtc_reverse(back[5]) + "|");
  Serial.println(pie + " |" + svtc_reverse(left[6]) + " " + svtc_reverse(left[7]) + " " + svtc_reverse(left[8]) + "|" + svtc_reverse(front[6]) + " " + svtc_reverse(front[7]) + " " + svtc_reverse(front[8]) + "|" + svtc_reverse(right[6]) + " " + svtc_reverse(right[7]) + " " + svtc_reverse(right[8]) + "|" + svtc_reverse(back[6]) + " " + svtc_reverse(back[7]) + " " + svtc_reverse(back[8]) + "|");
  Serial.println(pie + "             |" + svtc_reverse(bottom[0]) + " " + svtc_reverse(bottom[1]) + " " + svtc_reverse(bottom[2]) + "|");
  Serial.println(pie + "             |" + svtc_reverse(bottom[3]) + " " + svtc_reverse(bottom[4]) + " " + svtc_reverse(bottom[5]) + "|");
  Serial.println(pie + "             |" + svtc_reverse(bottom[6]) + " " + svtc_reverse(bottom[7]) + " " + svtc_reverse(bottom[8]) + "|");
}

//prints rows(1 for each side of cube) and their values in each column (1 value per address)
void print_raw() {
  print_array(top, SIZE); //0-8
  Serial.print("\n");

  print_array(left, SIZE); //9-17
  Serial.print("\n");

  print_array(front, SIZE); //18-26
  Serial.print("\n");

  print_array(right, SIZE); //27-35
  Serial.print("\n");

  print_array(back, SIZE); //36-44
  Serial.print("\n");

  print_array(bottom, SIZE); //45-53
  Serial.print("\n");
}

//prints a single array as a row (prints one side's values)
void print_array(int side[], int array_length) {
  for (int index = 0; index < array_length; index++) {
    Serial.print(side[index]);
    if (index < array_length - 1) {
      Serial.print("\t");
    }
  }
}

String svtc_reverse(int sticker_value) { //svtc returns a string from the input of sticker_value
  char x[3]; //creates a character array as setup for the sprintf function coming up
  sprintf(x, "%02d", sticker_value); //takes the value in the variable sticker_value and stores it into the char array x (3 length)
  String mystring(x);
  return (mystring);
}
