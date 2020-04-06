void ReadButtonState (int i)
{
  // Read buttons state
  // Left Button
  if ( digitalRead(buttonPin[i]) == true )
  {
    // если кнопка нажата, считаем тики
    buttonTicCount[i]++;   // +1 к счетчику состояния кнопки

    if ( buttonTicCount[i] >= TIME_BUTTON ) 
    {
      // состояние кнопки не мянялось в течение заданного времени - состояние кнопки стало устойчивым
      buttonClick[i] = true;
    }
  }
  else
  {
    buttonTicCount[i] = 0; // сброс счетчика подтверждений состояния кнопки
    buttonClick[i] = false;
    ignoreButton[i] = false; // сброс флага игнорирования нажатия кнопки. дабы небыло другой реакции на тоже самое нажатие.
  }
  //Serial.print(digitalRead(buttonPin[i]));
}
