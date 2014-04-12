import java.util.*;

public class tamagotchi {
  private boolean alive;
  private boolean hatched;
  private boolean egg;
  private boolean gender; // false = male; true = female;
  private int lvl;
  private int exp;
  private int happiness;
  private int hunger;
  
  public tamagotchi() {
    alive = true;
    hatched = false;
    egg = false;
    gender = randBoolean();
    lvl = 1;
    exp = 0;
    happiness = 0;
    hunger = 0;
  }
  
// Setters and Getters.  
  public boolean get_alive() {
    return alive;
  }
  public void set_alive(boolean b) {
    alive = b;
  }
  
  public boolean get_hatched() {
    return hatched;
  }
  public void set_hatched(boolean b) {
    hatched = b;
  }
  
  public boolean get_egg() {
    return egg;
  }
  public void set_egg(boolean b) {
    egg = b;
  }
  
  public String get_gender() {
    if(gender == false)
      return "Male";
    return "Female";
  }
  //No gender setter. Gender not changeable.
  
  public int get_lvl() {
    return lvl;
  }
  public void set_lvl(int i) {
     lvl = i;
  }
  
  public int get_exp() {
    return exp;
  }
  public void set_exp(int l) {
     exp = l;
  }
  
  public int get_happiness() {
    return happiness;
  }
  public void set_happiness(int i) {
     happiness = i;
  }
  
  public int get_hunger() {
    return hunger;
  }
  public void set_hunger(int i) {
     hunger = i;
  }
  
//Random Methods.  
  public boolean randBoolean() {
    Random r = new Random();
    int bool = r.nextInt(2);
    if(bool == 0)
      return false;
    return true;
  }
  public void reset() {
    alive = true;
    hatched = false;
    egg = false;
    gender = randBoolean();
    lvl = 1;
    exp = 0;
    happiness = 0;
    hunger =0;
  }   
}
  
  