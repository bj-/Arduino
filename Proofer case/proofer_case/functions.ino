
String addleaderzero(int val)
{
  // add leading zero if < 10
  //String addZero = "";
  String strVal = String(val);
  
  if ( val <= 9 )
  {
    return "0" + strVal;
  }
  else
  {
    return strVal;
  }
}
