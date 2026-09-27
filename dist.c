
int dist(int x1, int y1, int h1, int x2, int y2, int h2)
{
  int min,max,range,x = abs(x1-x2),y = abs(y1-y2);
  min = (x < y ? x : y );
  max = (x > y ? x : y );
  if(max != 0)
    range = max + ((min*min)/(2*max));
  else
    range = 0;

  return(range);
}
