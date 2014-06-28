#include <math.h>

typedef struct TrackVal {
	int left;
	int right;
} TrackVal;

int findQuadrant(int x, int y) {
  if(x == 0 || y == 0) {
    return 5; // obviously not a real quadrant and requires special attention
  } else if(x > 0) {
    if(y > 0) {
      return 1;
    } else {
      return 4;
    }
  } else {
    if(y > 0) {
      return 2;
    } else {
      return 3;
    }
  }
}

TrackVal calcTrackSpeed(int x, int y) {
  int leftTrackVal = 0;
  int rightTrackVal = 0;
  switch(findQuadrant(x, y)) {
    case 1:
      leftTrackVal = sqrt(pow(x, 2) + pow(y, 2));
      rightTrackVal = y;
      break;
    case 2:
      leftTrackVal = (x * -1);
      rightTrackVal = sqrt(pow(x, 2) + pow(y, 2));
      break;
    case 3:
      leftTrackVal = x;
      rightTrackVal = sqrt(pow(x, 2) + pow(y, 2)) * -1;
      break;
    case 4:
      leftTrackVal = sqrt(pow(x, 2) + pow(y, 2)) * -1;
      rightTrackVal = y;
      break;
    case 5: // either of the values is 0, not a quandrant but a special flag for this case
      if(x == 0) {
        leftTrackVal = y;
        rightTrackVal = y;
      } else {
        if(x < 0) {
          leftTrackVal = 0;
          rightTrackVal = x;
        } else {
          leftTrackVal = x;
          rightTrackVal = 0;
        }
      }
      break;
  }
  TrackVal curVal = {.left = leftTrackVal, .right = rightTrackVal};
  return curVal;
}

