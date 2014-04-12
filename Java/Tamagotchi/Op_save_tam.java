import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.Timer;
import javax.swing.event.*;
import java.util.*;

public class Op_save_tam {
  private JFrame frame;
  private JButton Start, Suicide, Ka_Ching;
  private JComboBox Food, Gifts;
  private JLabel foodlabel, giftlabel, moolah, happy, hungry, level, exp, purchase;
  private JPanel panel, panel2;
  private boolean win, restart, item_on_screen, moving_right, which, free_money, has_Clicked, poop_screen;//which: true = Food , false = Gifts
  private Screen screen;
  private String item; 
  private int money , counter , x , price , rand_money , button_life , item_counter , item_num , x_cor , y_cor, poop_counter;
  private Timer ticker;
  private tamagotchi player;
  private Image image_left, image_left2, image_right, image_right2, image_eating, image_eating2, image_exclamate, image_feces;
  private boolean[] feces;
  private ArrayList<Integer> feces_list;
  private String[] foods = {" " , "Fruit - 100 Yen" , "Snack - 100 Yen" , "Water - 100 Yen"};
  private String[] gifts = {" " , "Stuffed Animal - 100 Yen" , "Toy Car - 100 Yen" , "GBA - 1,000 Yen" ,  "Yugioh Cards - 10,000 Yen"};
  private final static int[] Yen = {100,1000,10000};
  private final static int[] food_exp = {5,5,10};
  private final static int[] food_hunger = {15,10,5};
  private final static int[] food_happiness = {5, 10, 10};
  private final static int[] gift_exp = {25, 25, 250, 2500};
  private final static int[] gift_happiness = {15, 15, 100, 500};
  private final static int[] exp_table =
  { 25 , 50 , 100 , 200 , 400 , 800 , 1600 , 3200 , 6400,
    12800 , 25600 , 51200 , 102400 , 204800 , 409600 , 819200 , 1638400 , 3276800 , 6553600 };
  
  public static void main(String[] args) {
    Op_save_tam game = new Op_save_tam();
  }
  
  public Op_save_tam() {
    player = new tamagotchi();
    player.set_alive(false);
    screen = new Screen(); 
    screen.addMouseListener(new ItemClicked());
    frame = new JFrame("Tamagotchi!");
    frame.setVisible(true);
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.setSize(800,450);
    frame.getContentPane().add(BorderLayout.CENTER, screen);
    frame.setResizable(false);
    createButtons(); 
    image_exclamate = new ImageIcon("exclamation_mark.gif").getImage();
    image_feces = new ImageIcon("image_feces.gif").getImage();
  }
/*                     */ 
/* THE SCREEN'S VISUAL */
/*                     */  
  class Screen extends JPanel {
    public void paintComponent(Graphics g) { 
      Graphics2D g2d = (Graphics2D) g;
      int width = this.getWidth();
      int height = this.getHeight();
      if(player.get_alive()) {
        Image image = new ImageIcon("forest.jpg").getImage();
        g.drawImage(image, 3, 4, width, height, this);
        g.drawString("Hi, master.", width - 100, 10);
         for(int i = 0; i < feces_list.size(); ++i) {
              g.drawImage(image_feces, feces_list.get(i), 320, 40, 30, this);
            }
        if(has_Clicked && item_counter < 10 && item_counter != 0) {
          if(player.get_gender().equals("Male")) { 
             if(item_counter % 2 == 0) image_eating = new ImageIcon("Mega_eating.gif").getImage();
             else  image_eating = new ImageIcon("Mega_eating2.gif").getImage();
          }
          else {
            if(item_counter % 2 == 1) image_eating = new ImageIcon("Fema_eating.gif").getImage();
            else image_eating = new ImageIcon("Fema_eating2.gif").getImage();
          }
          g.drawImage(image_eating, x_cor - 40, 250, 100, 100, this);
          Image image_food = new ImageIcon(item).getImage();
          g.drawImage(image_food, x_cor, 325, 25, 25, this);
          ++item_counter;
          if(item_counter == 10) {
            has_Clicked = false;
            purchase.setVisible(false);
            foodlabel.setVisible(true);
            Food.setVisible(true);
            giftlabel.setVisible(true);
            Gifts.setVisible(true);
            if(which){
              player.set_hunger(player.get_hunger() + food_hunger[item_num]);
              player.set_happiness(player.get_happiness() + food_happiness[item_num]);
              player.set_exp(player.get_exp() + food_exp[item_num]);
            }
            else{
              player.set_happiness(player.get_happiness() + gift_happiness[item_num]);
              player.set_exp(player.get_exp() + gift_exp[item_num]);
            }
          }
        }
        else {
          if(has_Clicked) {
          Image image_food = new ImageIcon(item).getImage();
          g.drawImage(image_food, x_cor, 325, 25, 25, this);
          ++item_counter;
          }
          if(player.get_gender().equals("Male")) {
            image_left = new ImageIcon("Mega_left.gif").getImage();
            image_right = new ImageIcon("Mega_right.gif").getImage();
            image_left2 = new ImageIcon("Mega_left2.gif").getImage();
            image_right2 = new ImageIcon("Mega_right2.gif").getImage();
          }
          else {
            image_left = new ImageIcon("Fema_left.gif").getImage();
            image_right = new ImageIcon("Fema_right.gif").getImage();
            image_left2 = new ImageIcon("Fema_left2.gif").getImage();
            image_right2 = new ImageIcon("Fema_right2.gif").getImage();
          }
          if(moving_right) {
            if(x % 2 == 0) {
              g.drawImage(image_right, x, 250, 100, 100, this);
              if(has_Clicked) g.drawImage(image_exclamate, x + 25, 150, 50, 90, this);
            }
            else {
                g.drawImage(image_right2, x, 250, 100, 100, this);
                if(has_Clicked) g.drawImage(image_exclamate, x + 25, 150, 50, 90, this);
            }
            if(player.get_hunger() >= 100 && feces[x] == false) {
                int feces_counter = 0;
                player.set_hunger(player.get_hunger() - 20);
                for(int i = x; i < feces.length && feces_counter < 10; ++i) {
                  if(i == x) feces_list.add(i);
                  feces[i] = true;
                  ++feces_counter;
                }
              }
            x += 13;
            if(feces[x])
              player.set_happiness(player.get_happiness() - 5);
          }
            if(x >= 700) {
               moving_right = false;
          }
          if(!moving_right) {
            if(x % 2 == 0) { 
              g.drawImage(image_left, x, 250, 100, 100, this);
              if(has_Clicked) g.drawImage(image_exclamate, x + 25, 150, 50, 90, this);
            }
            else {
                g.drawImage(image_left2, x, 250, 100, 100, this);
                if(has_Clicked) g.drawImage(image_exclamate, x + 25, 150, 50, 90, this);
            }
            if(player.get_hunger() >= 100 && feces[x] == false) {
                int feces_counter = 0;
                poop_counter++;
                poop_screen = true;
                player.set_hunger(player.get_hunger() - 20);
                for(int i = x; i < feces.length && feces_counter < 40; ++i) {
                  if(i == x) feces_list.add(i);
                  feces[i] = true;
                  ++feces_counter;
                }
              }
            x -= 13;
            if(feces[x]) player.set_happiness(player.get_happiness() - 5);
          }
            if(x <= 0) { 
               moving_right = true;
                  }
          if(counter % 40 == 39) {
          rand_money = randMoney();
          button_life = 10;
          free_money = true;
          Ka_Ching.setText("Free Money! " + rand_money + " Yen. Click to recieve (" + button_life + ").");
          Ka_Ching.setVisible(true);
          Suicide.setVisible(false);
          foodlabel.setVisible(false);
          Food.setVisible(false);
          giftlabel.setVisible(false);
          Gifts.setVisible(false);
        }
          if(counter % 10 == 9) {
            player.set_happiness(player.get_happiness() - 1);
          }
          if(counter % 10 == 9) {
            player.set_hunger(player.get_hunger() - 1);
          }
        }
      }
      else {
        super.paintComponent(g);
        /* g.setColor(Color.WHITE);
        g.fillRect(0, 0, width, height);*/
        g.drawString("Press START", 100, 100);
        g.drawString("You win by reaching level 20 and upon winning, you shall be awared. Call Us.", 100, 200);
        g.drawString("We recommend your gender be female, so keep suiciding till it's a female because your computer is not as fast as ours.", 100, 300);
      }
    }
  }
/*             */
/* MOUSE CLICK */
/*             */  
  class ItemClicked implements MouseListener{
    public void mousePressed(MouseEvent e) {}
    public void mouseReleased(MouseEvent e) {}
    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}
    
    public void mouseClicked(MouseEvent e) {
      x_cor = e.getX();
      y_cor = e.getY();
      if(poop_screen && poop_counter > 0) {
        for(int x = 0; x < feces_list.size(); x++) {
          int f = feces_list.get(x);
          if((x_cor - f <= 39 && x_cor - f >= 0) && (y_cor >= 320 && y_cor <= 350)) {
            for(int b = 0; b < 40; b++)
              feces[f + b] = false ;
            feces_list.remove(x);
            break;
          }
        }
      }                          
      if(item_on_screen) {
        if(money >= price) {
          has_Clicked = true;
          money -= price;
        }
      if(which){
        Food.setSelectedIndex(0);
      }
      else{
        Gifts.setSelectedIndex(0);
      }
      item_on_screen = false;
      purchase.setText(" ");
    }
  }
  }
/*         */  
/* BUTTONS */
/*         */    
  class StartPressed implements ActionListener {
    public void actionPerformed(ActionEvent e) {
       Start();
    }
  }
  
  class SuicidePressed implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      player.set_alive(false);
      Suicide.setVisible(false);
      foodlabel.setVisible(false);
      Food.setVisible(false);
      giftlabel.setVisible(false);
      Gifts.setVisible(false);
      restart = true;
      Start.setVisible(true);
      Start.setText("Click to restart game.");
      ticker.stop();
      player.set_alive(false);
      screen.repaint();
    }
  }
  
  class FoodPressed implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      which = true;
      item_counter = 0;
      String food = (String) Food.getSelectedItem();
      if(!food.equals(" ")) {
        foodlabel.setVisible(false);
        Food.setVisible(false);
        giftlabel.setVisible(false);
        Gifts.setVisible(false);
        purchase.setText("You have purchased: " + food + ". Please place the item on the screen.");
        purchase.setVisible(true);
      }
      if(food.equals("Fruit - 100 Yen")){
        item = "Fruit.gif";
        item_num = 0;
        item_on_screen = true; 
        price = 100;
      }
      if(food.equals("Snack - 100 Yen")){
        item = "Snack.gif";
        item_num = 1;
        item_on_screen = true;
        price = 100;
      }
      if(food.equals("Water - 100 Yen")){
        item = "Water.gif";
        item_num = 2;
        item_on_screen = true;
        price = 100;
      }    
    }
  }
  
  class GiftsPressed implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      which = false;
      item_counter = 0;
      String gift = (String) Gifts.getSelectedItem();
      if(!gift.equals(" ")) {
        foodlabel.setVisible(false);
        Food.setVisible(false);
        giftlabel.setVisible(false);
        Gifts.setVisible(false);
        purchase.setText("You have purchased: " + gift + ". Please place the item on the screen.");
        purchase.setVisible(true);
      }
      if(gift.equals("Stuffed Animal - 100 Yen")) {
        item = "Stuffed Animal.gif";
        item_num = 0;
        item_on_screen = true;
        price = 100;
      }
      if(gift.equals("Toy Car - 100 Yen")) {
        item = "Toy Car.gif";
        item_num = 1;
        item_on_screen = true;
        price = 100;
      }
      if(gift.equals("GBA - 1,000 Yen")) {
        item = "GBA.gif";
        item_num = 2;
        item_on_screen = true;
        price = 1000;
      }
      if(gift.equals("Yugioh Cards - 10,000 Yen")){
        item = "Yugioh.gif";
        item_num = 3;
        item_on_screen = true;
        price = 10000;
      }
    }
  }
    
    class ChingPressed implements ActionListener {
      public void actionPerformed(ActionEvent e) {
        money += rand_money;
        Ka_Ching.setVisible(false);
        Suicide.setVisible(true);
        foodlabel.setVisible(true);
        Food.setVisible(true);
        giftlabel.setVisible(true);
        Gifts.setVisible(true);
        button_life = 3;
        free_money = false;
        
      }
    }
    
  class RunTicker implements ActionListener {
    public void actionPerformed(ActionEvent event) {
      Step();
    }
  }

/*         */  
/* METHODS */
/*         */  
  public void createButtons() {
    panel = new JPanel();
    frame.getContentPane().add(BorderLayout.SOUTH, panel);
    panel.setVisible(true);
    Start = new JButton("Start Game");
    panel.add(Start);
    Start.addActionListener(new StartPressed());
    Suicide = new JButton("Suicide");
    panel.add(Suicide);
    Suicide.setVisible(false);
    Suicide.addActionListener(new SuicidePressed());
    Ka_Ching = new JButton();
    panel.add(Ka_Ching);
    Ka_Ching.setVisible(false);
    Ka_Ching.addActionListener(new ChingPressed());
    foodlabel = new JLabel("          Buy food : ");
    foodlabel.setVisible(false);
    panel.add(foodlabel);
    Food = new JComboBox(foods);
    panel.add(Food);
    Food.setVisible(false);
    Food.addActionListener(new FoodPressed());
    giftlabel = new JLabel("          Buy gift : ");
    giftlabel.setVisible(false);
    panel.add(giftlabel);
    Gifts = new JComboBox(gifts);
    panel.add(Gifts);
    Gifts.setVisible(false);
    Gifts.addActionListener(new GiftsPressed());
    purchase = new JLabel("You have purchased: ");
    purchase.setVisible(false);
    panel.add(purchase);
    
  }
  
  public void createMonitor() {
    panel2 = new JPanel();
    frame.getContentPane().add(BorderLayout.NORTH, panel2);
    moolah = new JLabel("Money:"+money+" Yen     ");
    panel2.add(moolah);
    happy = new JLabel("Happiness: "+player.get_happiness()+"     ");
    panel2.add(happy);
    hungry = new JLabel("Hunger: "+player.get_hunger()+"     ");
    panel2.add(hungry);
    level = new JLabel("Level: " + player.get_lvl() + "     ");
    panel2.add(level);
    exp = new JLabel("Exp: " + player.get_exp() / exp_table[player.get_lvl() - 1] * 100 + " %");
    panel2.add(exp);
  }
  
  private void Start() {
    if (restart == true){
      restart = false;
      Start.setVisible(false);
      Start.setText("Start");
    }
    Start.setVisible(false);
    Suicide.setVisible(true);
    foodlabel.setVisible(true);
    Food.setVisible(true);
    giftlabel.setVisible(true);
    Gifts.setVisible(true);
    screen.repaint();
    x = 0;
    moving_right = true;
    feces = new boolean[screen.getWidth()];
    for(int i = 0; i < feces.length; ++i)
      feces[i] = false;
    feces_list = new ArrayList<Integer>();
    player = new tamagotchi();
    player.set_happiness(50);
    player.set_hunger(50);
    player.set_alive(true);
    createMonitor();
    ticker = new Timer(10, new RunTicker());
    ticker.setDelay(750);
    ticker.start();
    counter = 0;
    poop_counter = 0;
    money = 50000;
    item_on_screen = false;
    restart = false;
    win = false;    
    free_money = false;
    which = true;
    poop_screen = false;
  }
  
  private void Step() {
    screen.repaint();
    ++counter;
    moolah.setText("Money: " + money + " Yen     ");
    happy.setText("Happiness: " + player.get_happiness() +"     ");
    hungry.setText("Fullness: " + player.get_hunger() +"     ");
    level.setText("Level: " + player.get_lvl() + "     ");
    exp.setText("Exp: " + (int)(player.get_exp() / (double) exp_table[player.get_lvl() - 1] * 100) + " %");
    if (free_money) {
      if(button_life < 0) {
        Ka_Ching.setVisible(false);
        Suicide.setVisible(true);
        foodlabel.setVisible(true);
        Food.setVisible(true);
        giftlabel.setVisible(true);
        Gifts.setVisible(true);
        button_life = 3;
        free_money = false;
      }
      Ka_Ching.setText("Free Money! " + rand_money + " Yen. Click to recieve (" + button_life + ").");
      button_life--;
    }
    if(player.get_exp() / exp_table[player.get_lvl() - 1] >= 1) {
      player.set_exp(player.get_exp() - exp_table[player.get_lvl() - 1]);
      player.set_lvl(player.get_lvl() + 1 );
  }
  }
  
  private int randMoney(){
    Random r = new Random();
    int mon = r.nextInt(111);
    if( mon == 0 ) 
      return Yen[2];
    if( mon >= 1 && mon <= 10 )
      return Yen[1];
    return Yen[0];
  }
}
  
  