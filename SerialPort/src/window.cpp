#include <cstddef>
#include <sys/ioctl.h>
#include <ncurses.h> 
#include <locale.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <pthread.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>

#include "../include/window.hpp"

#include "../include/method.hpp"

int ui_lines;
int ui_cols;
bool quit=false;
std::string device;
WINDOW *rx_data_win,*tx_data_win,*port_info_win;

int ui_init(){
    setlocale(LC_ALL, "");
    // initialize the terminal and clear
    initscr();
    // immediately read the character user input
    // accept suspend ^Z and interupt ^C
    cbreak();
    // no echo user's input
    noecho();
    // donot convert line break to \n or \r 
    nonl();
    // set cursor
    curs_set(0);
    // function keys are permitted
    // for example,F1,KEY_LEFT,etc.
    //keypad(stdscr, TRUE);
    return 0;
}

// chtype context for output single character
int ui_print(int attrs,
             int pos_row,
             int pos_col,
             chtype context){
    attron(attrs);
    mvaddch(pos_row,pos_col, context);
    attroff(attrs);
    return 0;
}

// char * context for output character string
int ui_print(int attrs,
             int pos_row,
             int pos_col,
             const char * context){
    attron(attrs);
    mvaddstr(pos_row,pos_col, context);
    attroff(attrs);
    return 0;
}

int ui_print_center(int line,
                     const char * context){
    int width;
    //move(line,0);
    width=strlen(context);
    clrtoeol();
    mvaddstr(line, (ui_cols-width)/2, context);
    return 0;
}

int ui_print_center(int attrs,
                    int line,
                    const char * context){
    int width;
    //move(line,0);
    width=strlen(context);
    clrtoeol();
    attron(attrs);
    mvaddstr(line, (ui_cols-width)/2, context);
    attroff(attrs);
    return 0;
}

int ui_print_center(int attrs,
                    int line,
                    std::string context){
    int width;
    //move(line,0);
    width=context.size();
    clrtoeol();
    attron(attrs);
    const char * contextStr=context.c_str();
    mvaddstr(line,(ui_cols-width)/2,contextStr); 
    attroff(attrs);
    return 0;
}

/* line = row2 - row1 + 1
   width = col2 - col1 + 1
 */
int ui_draw_data_frame(int toprow,
                       int topcol,
                       int line,
                       int width){
    // left top frame 
    ui_print(0, toprow, topcol, ACS_ULCORNER);
    // center top frame 
    for(int i=topcol+1 ; i<topcol+width-1; i++){
        ui_print(0, toprow, i, ACS_HLINE);
    }
    // right top frame 
    ui_print(0, toprow, topcol+width-1, ACS_URCORNER);
    // vertical frame 
    for(int i=toprow+1; i<toprow+line-1; i++){
        ui_print(0, i, topcol, ACS_VLINE);
        ui_print(0, i, topcol+width-1, ACS_VLINE);
    }
    // left bottom frame 
    ui_print(0, toprow+line-1, 1, ACS_LLCORNER);
    // center bottom frame 
    for(int i=topcol+1 ; i<topcol+width-1; i++){
        ui_print(0, toprow+line-1, i, ACS_HLINE);
    }
    // right bottom frame 
    ui_print(0, toprow+line-1, topcol+width-1, ACS_LRCORNER);
    return 0;
}

// line for rows
// width for cols
int ui_draw_frame(int line,int width){
    // Program title at row 1
    ui_print_center(0, 1, "Serial Port Utils by LeeSin");
    // Port Info
    ui_print(0, 2, 2, "Port Info");
    /* Port Info frame
       row 3 to row 5
       col 1 to col width-1
     */
    ui_draw_data_frame(3, 1, 3, width-1);
    /* Port Info window
       row 4
       col 2 to col width-2
     */
    port_info_win = derwin(stdscr, 1, width-3, 4, 2);
    // rx title at row 6,col 2
    ui_print(0, 6, 2, "RX Data");
    /* rx data frame
       row 7 to row line-5
       col 1 to col width-1
     */
    ui_draw_data_frame(7, 1, line-11, width-1);
    /* rx data window 
       row 8 to row line-6
       col 2 to col width-2
     */
    rx_data_win = derwin(stdscr, line-13, width-3, 8, 2);
    
    // tx title at row line-4,col 2
    ui_print(0, line-4, 2, "TX Data");
    /* tx data frame
       row line-3 to row line-1
       col 1 to col width-1
     */
    ui_draw_data_frame(line-3, 1, 3, width-1);
    /* tx data window 
       row line-2 
       col 2 to col width-2
     */
    tx_data_win = derwin(stdscr, 1, width-3, line-2, 2);
    //delay_output(1);
    nodelay(tx_data_win,true);
    return 0;
}

int ui_insert(WINDOW *win,int ch){
    int y,x;
    getyx(win, y, x);
    winsch(win, ch);
    wmove(win, y, ++x);
    wrefresh(win);
    return 0;
}

int rxdata(){
    static int x=0,y=0;
    //static bool flag=true;//first enter need position setup
    //if(flag){
        wmove(rx_data_win,0,0);
        //flag=false;
    //}
    
    if(!PgSerial.available()){
        return 0;
    }

    //werase(rx_data_win);
    //FILE *fp;
    //fp = fopen("rxdata.txt","a+");
        //return 0;
    std::vector<std::string> result = PgSerial.readlines(65536,"\n");
    std::vector<std::string>::iterator it = result.begin();

    for(int i=0;i<result.size();i++){
        //fprintf(fp,it->c_str());
        for(int j=0;j<result[i].size();j++){
            if(result[i][j] == '\n' || result[i][j] == '\r')
                continue;
            wmove(rx_data_win,y,x++);
                //winsch(rx_data_win,'~');
            winsch(rx_data_win,result[i][j]);
            
        }
        x=0;
        y++;
    }
    
    wrefresh(rx_data_win);
    //y=0;
    //fclose(fp);
    //wrefresh(rx_data_win);
    //pg_sleep(100);
    return 0;
}

int getLastChar(WINDOW *win){
    int y,x,max_y,max_x;
    getyx(tx_data_win,y,x);
    getmaxyx(tx_data_win,max_y,max_x);
    x=max_x-1;
    wmove(tx_data_win,y,x);
    while(isspace(winch(tx_data_win)) && x > 0)
        wmove(tx_data_win,y,--x);
    wmove(tx_data_win,y,0);
    return x;
}

int txdata(){
    //winsch(tx_data_win,'Z');
    //wrefresh(tx_data_win);
    wmove(rx_data_win,0,0);
    werase(rx_data_win);
    wrefresh(rx_data_win);
    int y,x,last_x;
    last_x=getLastChar(tx_data_win);    
    if(last_x==0){
        winsch(tx_data_win,'A');
        wrefresh(tx_data_win);
        return 0;
    }
    std::string data="";
    for(int i=0;i<=last_x;i++){
        wmove(tx_data_win,0,i);
        data+=(char)winch(tx_data_win);
    }    
    data+='\n';
    PgSerial.write(data);
    wmove(tx_data_win,0,0);
    werase(tx_data_win);
    wrefresh(tx_data_win);
    return 0;
}

int ui_loop(){
    int y,x;//the position of the cursor
    int ch=0;
    wmove(tx_data_win, 0, 0);
    while(quit == false){
        rxdata();
        ch=wgetch(tx_data_win);  
        if(ch >= 65 && ch <= 126){ //32-126
            ui_insert(tx_data_win,ch);
            continue;
        }
        switch (ch) {
            case KEY_LEFT:
                getyx(tx_data_win, y, x);
                wmove(tx_data_win, y, --x);
                break;
            case KEY_RIGHT:
                getyx(tx_data_win, y, x);
                wmove(tx_data_win, y, ++x);
                break;
            case VK_EDIT:
            case VK_ENTER:
                txdata();
                //send the message
                break;
            case VK_ESCAPE:
                quit = true;
                break;
            case KEY_BACKSPACE:
                werase(rx_data_win);
                wrefresh(rx_data_win);
                break;
            default:
                break;
        }
    }
    return 0;
}

int ui_show_port_info(){
    std::vector<std::string> sysfs_info = get_sysfs_info(device);
    std::string friendly_name = sysfs_info[0];
    std::string hardware_id = sysfs_info[1];
    int x=0;
    wmove(port_info_win,0,0);
    //int length=device.length() + 1;
    //char* p = strcpy(new char[length], device.c_str());
    //winnstr(port_info_win,p,device.length() + 1);
    winsstr(port_info_win,(char*)device.c_str());
    //delete[] p;
    wmove(port_info_win, 0, x+=(device.size()+1));
    for(int i=0;i<2;i++){
         winsstr(port_info_win,(char*)sysfs_info[i].c_str());
         wmove(port_info_win, 0, x+=(sysfs_info[i].size()+1));
    }
    wrefresh(port_info_win);
    return 0;
}

int ui_run(){
    struct winsize ws;
    /* Get the configuration of the terminal
       ioctl defined in <sys/ioctl.h>
       TIOCGWINSZ defined in <unistd.h>
     */
    ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws);
    ui_lines=ws.ws_row;
    ui_cols=ws.ws_col;
    ui_draw_frame(ui_lines,ui_cols);
    scrollok(rx_data_win,TRUE);
    ui_show_port_info();
    refresh();
    keypad(stdscr, TRUE);
    keypad(rx_data_win, TRUE);
    keypad(tx_data_win, TRUE);
    ui_loop();
    //getch();
    return 0;
}

// int thread_run(){
//     int res=0;
//     pthread_t thread_gui,thread_rx;
//     void* thread_result;
//     ui_run();
//     // res = pthread_create(&thread_gui, NULL, ui_run, NULL);
//     // if (res != 0) {
//     //     printw("Thread gui failed.\n");
//     //     return -1;
//     // }
//     // res = pthread_create(&thread_rx, NULL, rxdata, NULL);
//     // if (res != 0) {
//     //    printw("Thread rx data failed.\n");
//     //    return -1;
//     // }

//     // block the main thread until
//     //res = pthread_join(thread_rx, &thread_result);

//     return 0;
// }

int decode_args(int argc,char *argv[]){
    if(argc<2){
        print_usage();
        return -1;
    }

    // Argument 1 is the serial port or enumerate flag
    std::string port(argv[1]);

    if( port == "-e" ) {
        enumerate_ports();
        return 1;
    }else if (argc < 3) {
        print_usage();
        return -1;
    }

    // Argument 2 is the baudrate
    unsigned long baud = 0;
    sscanf(argv[2], "%lu", &baud);

    setupSerial(port,baud);
    device=port;
    return 0;
}

int test(){
    while(true){
        int i=0;
        if(!PgSerial.available()){
            pg_sleep(20);
            continue;
        }
        std::vector<std::string> result = PgSerial.readlines(65536,"\n");
        std::cout << result.size() << std::endl;
        std::vector<std::string>::iterator it=result.begin(); 
        for(; it != result.end(); ++it,++i)
        {
            std::cout << it->c_str() << "This is "<< i <<" line"<< std::endl;
        }
        pg_sleep(20);
    }
    return 0;
}

int test2(){
    std::string a=" \\ | /                  ";
    std::string b="- RT - Operating System ";
    std::string c=" / | \\                  ";
    std::string d="2006 - 2020 build in 2023 Jan 10"; 
    wmove(stdscr,1,1);
    winsstr(stdscr,(char*)a.c_str());
    wmove(stdscr,2,1);
    winsstr(stdscr,(char*)b.c_str());
    wmove(stdscr,3,1);
    winsstr(stdscr,(char*)c.c_str());
    wmove(stdscr,4,1);
    winsstr(stdscr,(char*)d.c_str());
    wrefresh(stdscr);
    getchar();
    return 0;
}

int main(int argc, char *argv[]) {
    int res=0;
    res=decode_args(argc,argv);
    if(res==-1)
        return res;
    if(res==1)
        return 0;
    ui_init(); 
    res=ui_run();
    //test2();
    endwin(); 
    //test();
    return res;
}
