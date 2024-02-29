#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <MLV/MLV_all.h>
#define Taille_x 1000
#define Taille_y 1000
#define R 400
#define xc Taille_x/2
#define yc Taille_y/2

typedef struct pnt{
int x;
int y;
struct pnt *suiv;
}Point;

typedef struct vrtx{ 
Point *s;
struct vrtx *prev; 
struct vrtx *next; 
}Vertex, *Polygon;

typedef struct{
Polygon pol;
int curlen;
int maxlen;
}Convexhull;

/*fonction qui prend en parametre une liste chainee de points 
et un nouveau point et remplie la liste de points avec un nouveau point donné*/
void rempli_lst(Point *lst, Point *new);

/*fonction qui prend en parametre 3 points et qui renvoie 1 si C est à droite de A-B, 0 sinon*/
int position_pnt(Point *A, Point *B, Point *C);

/*fonction qui prend en parametre une liste chainee de points et qui construit le convexe*/
Convexhull *initialise(Point *lst);

/*fonction  qui prend en parametres deux entiers x,y et qui alloue le point*/
Point *alloue_pnt(int x, int y);

/*fonction qui prend en parametre la liste des points et qui la libère*/
void free_point(Point *lst);

/*fonction qui prend un parametre un point et qui alloue un vtx et y ajoute un pnt*/
Polygon allouer_vtx(Point *pnt);

/*fonction qui prend en parametre un vertex de type Polygon et qui le libere*/
void free_vtx(Polygon vtx);

/*fonction qui prend en parametre un pointeur vers une structure Convexehull et 
qui balaye vers l’avant et supprime un vtx du convexe si il en fait plus partie et le libere*/
void netoy_avant(Convexhull *conv);

/*fonction qui prend en parametre un pointeur vers une structure Convexehull 
et qui balaye vers l’arrière et supprime un vtx du convexe si il en fait plus partie et le libere*/
void netoy_arriere(Convexhull *conv);

/*fonction qui prend en parametre un pointeur vers une structure Convexehull 
et un pointeur vers une structure Point qui est un point C et qui gere un nouveau 
point par rapport au convexe et renvoie 1 si il l’a ajouté et 0 sinon. 
La fonction crée un nouveau convexe si elle doit ajouter le point*/
int gere_pnt(Convexhull *conv, Point *C);

/*fonction qui prend en parametre la lsite des points et qui dessine graphiquement les points en blanc*/
void affiche_pnt(Point *lst);

/*fonction qui prend en parametre un convexe et qui dessine graphiquement le convexe*/
void affiche_conv(Convexhull* conv);

/*fonction qui prend en parametre un entier figure representant la figure choisie 
et un entier r representant le rayon et qui génere aléatoirement un point selon un rayon et une figure donné*/
Point *genere_pnt(int figure, int r);

/*fonction qui prend en parametre trois entiers representant respectivement le type 
d'affichage choisi ansi que la figure choisie et un nombre de points et qui affiche le convexe 
d'une figure donné (cercle ou carré) selon un type d'affichage (graphique ou terminal)*/
void graphique(int type, int figure, int NB_pnt);

/*fonction qui prend en parametre un entier pointeur permettant de savoir si le joueur 
veut continuer de jouer et qui affiche une interface graphique permettant à l'utilisateur 
de dessiner un polygone en cliquant avec la souris sur la fenêtre*/
void phase1(int *jouer);

/*fonction qui prend en parametre deux entiers pointeurs representant le choix du joueur 
et une valeur permettant de savoir si le joueur veut continuer de jouer et qui permet à 
l'utiliateur de choisir graphiquement entre la partie manuelle ou automatique*/
void choix(int *choix, int *jouer);

/*fonction qui prend en parametre deux entiers pointeurs representant le type d'affichage 
choisi et une valeur permettant de savoir si le joueur veut continuer de jouer et qui 
affiche une interface graphique permettant à l'utilisateur de choisir entre afficher les 
résultats dans une fenêtre graphique ou dans le terminal.*/
void choix_affichage(int *type, int *jouer);

/*fonction qui prend en parametre deux entiers pointeurs representant le choix du joueur 
et une valeur permettant de savoir si le joueur veut continuer de jouer et qui affiche 
l'interface graphique permettant au joeur de choisir sa figure (cercle ou carre)*/
void choix_fig(int *figure, int *jouer);

/*fonction qui prend en parametre un entier pointeur representant un nombre de 
points et verifie si la saisie de l'utilisateur est bien un entier*/
void NBR(int *NB_pnt);



/*########################################################################
                            FONCTIONS POUR LE JEUX
########################################################################*/



void free_point(Point *lst){
    if(lst == NULL){
        return ;
    }
    Point *tmp=lst->suiv;
    while(tmp){
        free(lst);
        lst=tmp;
        tmp=lst->suiv;
    }
    free(lst);
}


void free_vtx(Polygon vtx){
    Polygon courant = vtx;
    while(courant!=vtx){
        Polygon tmp = courant->next;
        free(courant);
        courant=tmp;
    }
}


void affiche_pnt(Point *lst){
    for(;lst;lst=lst->suiv){
        MLV_draw_filled_circle(lst->x,lst->y,5,MLV_COLOR_RED);
    }
}


void affiche_conv(Convexhull* conv){
    int tab_x[conv->curlen];
    int tab_y[conv->curlen];
    Polygon pol=conv->pol;
    for(int i = 0; i<conv->curlen; i++){
        tab_x[i] = pol->s->x;
        tab_y[i] = pol->s->y;
        MLV_draw_filled_circle(pol->s->x,pol->s->y,5,MLV_rgba(0, 200, 100, 255));
        pol = pol->next;
    }
    MLV_draw_filled_polygon(tab_x, tab_y, conv->curlen, MLV_rgba(0, 200, 100, 30));
    MLV_draw_polygon(tab_x, tab_y, conv->curlen, MLV_rgba(0, 200, 100, 255));
}



Point *alloue_pnt(int x, int y){
    Point *new = (Point*)malloc(sizeof(Point));
    if(new == NULL){
        return NULL;
    }
    new->x=x;
    new->y=y;
    new->suiv=NULL;
    return new;
}


Polygon allouer_vtx(Point *pnt){
    Polygon new = (Vertex*)malloc(sizeof(Vertex));
    if (new==NULL){
        return NULL;
    }
    new->s=pnt;
    new->next=NULL;
    new->prev=NULL;
    return new;
}


void rempli_lst(Point *lst, Point *new){
    if(lst==NULL){
        lst=new;
    }
    else{
        for(;lst->suiv;lst = lst->suiv){}
        lst->suiv=new;
    }
}



Convexhull *initialise(Point *lst){
    Convexhull *conv = (Convexhull *)malloc(sizeof(Convexhull));
    if(conv==NULL){
        return NULL;
    }
    Polygon A = allouer_vtx(lst);
    lst=lst->suiv;
    Polygon B = allouer_vtx(lst);
    lst=lst->suiv;
    Polygon C = allouer_vtx(lst);
    conv->curlen=3;
    conv->maxlen=3;
    if (A->s->y > B->s->y){
        Polygon tmp = A;
        A=B;
        B=tmp;
    }
    if(position_pnt(A->s, B->s, C->s)==1){
        C->next=A;
        B->next=C;
        A->next=B;
        C->prev=B;
        B->prev=A;
        A->prev=C;
        conv->pol=A;
    }
    else{
        A->next=C;
        C->next=B;
        B->next=A;
        C->prev=A;
        B->prev=C;
        A->prev=B;
        conv->pol=C;
    }
    return conv;
}


int position_pnt(Point *A, Point *B, Point *C){
    if((((B->x - A->x) * (C->y - A->y)) - ((B->y - A->y) * (C->x - A->x))) <= 0){
        return 1;
    }
    return 0;
}


void netoy_avant(Convexhull *conv){
    Polygon A = conv->pol;
    Polygon B = A->next;
    Polygon C = B->next;
    while(position_pnt(A->s,B->s,C->s)==0){
        A->next=C;
        C->prev=A;
        free(B);
        B = C;
        C=C->next;
        conv->curlen-=1;
    }
}


void netoy_arriere(Convexhull *conv){
    Polygon A = conv->pol;
    Polygon B = A->prev;
    Polygon C = B->prev;
    while(position_pnt(A->s,B->s,C->s)==1){
        A->prev=C;
        C->next=A;
        free(B);
        B = C;
        C=C->prev;
        conv->curlen-=1;
    }
}



int gere_pnt(Convexhull *conv, Point *C){
    Polygon depart = conv->pol;
    Polygon A = conv->pol;
    Polygon B = A->next;
    for(;B!=depart;){
        if(position_pnt(A->s,B->s,C)==0){
            Polygon new=allouer_vtx(C);
            A->next=new;
            new->prev=A;
            new->next=B;
            B->prev=new;
            conv->curlen+=1;
            conv->pol=new;
            netoy_arriere(conv);
            netoy_avant(conv);
            return 1;
        }
        A=B;
        B=B->next;
    }
    if(position_pnt(A->s,B->s,C)==0){
        Polygon new=allouer_vtx(C);
        A->next=new;
        new->prev=A;
        new->next=B;
        B->prev=new;
        conv->curlen+=1;
        conv->pol=new;
        netoy_arriere(conv);
        netoy_avant(conv);
        return 1;
    }
    return 0;
}



/*########################################################################
                    FONCTIONS POUR LES DIFFÉRENTES PARTIES
########################################################################*/




void phase1(int *jouer){
    MLV_clear_window(MLV_COLOR_WHITE);
    MLV_draw_text_box(1,1,100,50,"QUITTER",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_draw_text_box(800,1,200,50,"RECOMMENCER",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_draw_text_box(450,1,100,50,"MENU",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);

    MLV_update_window();
    Point *lst = NULL;
    Convexhull *conv = NULL;
    int i = 0;
    int x = 0;
    int y = 0;
    while(*jouer){
        i++;
        MLV_clear_window(MLV_COLOR_WHITE);
        MLV_draw_text_box(1,1,100,50,"QUITTER",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
        MLV_draw_text_box(800,1,200,50,"RECOMMENCER",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
        MLV_draw_text_box(450,1,100,50,"MENU",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
        MLV_wait_mouse(&x,&y);
        if(x>1 && x<101 && y>1 && y<51){
            *jouer = 0;
            i-=1;
            break;
        }
        if(x>800 && x<1000 && y>1 && y<51){
            phase1(jouer);
            break;
        }
        if(x>450 && x<550 && y>1 && y<51){
            break;
        }
        
        Point *new_pnt = alloue_pnt(x, y);
        if(i==1){MLV_update_window();
            lst=new_pnt;
        }
        else{
            rempli_lst(lst, new_pnt);
        }
        affiche_pnt(lst);
        if(i==3){
            conv=initialise(lst);
            affiche_conv(conv);
        }
        if(i>3){
            gere_pnt(conv,new_pnt);
            affiche_conv(conv);
        }
        MLV_update_window();
    }
    free_point(lst);
    if(i>=3){
        free_vtx(conv->pol);
    }
}


Point *genere_pnt(int figure, int r){
    int x = xc-r + rand()%(2*r);
    int y = yc-r + rand()%(2*r);
    if (figure == 1){
        while(sqrt((x - xc)*(x - xc) + (y - yc) * (y - yc))>r){
            x = xc-r + rand()%(2*r);
            y = yc-r + rand()%(2*r);
        }
    }
    Point *new = alloue_pnt(x,y);
    return new;
}


void graphique(int type, int figure, int NB_pnt){
    MLV_clear_window(MLV_COLOR_WHITE);
    Point *lst = NULL;
    Convexhull *conv = NULL;
    int i = 0;
    int r_max = 5;
    while(i<NB_pnt){
        i++;
        if(type == 1){
            MLV_clear_window(MLV_COLOR_WHITE);
        }
        if(figure == 1 && type ==1){
            MLV_draw_circle(xc,yc,r_max,MLV_COLOR_RED);
        }
        else{
            if(figure == 0 && type ==1){
                MLV_draw_rectangle(xc-r_max,yc-r_max,r_max*2,r_max*2,MLV_COLOR_RED);
            }
        }
        Point *new_pnt = genere_pnt(figure,r_max);
        if(i==1){
            lst=new_pnt;
        }
        else{
            rempli_lst(lst, new_pnt);
        }
        if(type == 1){affiche_pnt(lst);}
        if(i==3){
            conv=initialise(lst);
            if(type == 1){affiche_conv(conv);}
        }
        if(i>3){
            gere_pnt(conv,new_pnt);
            if(type == 1){affiche_conv(conv);}
        }
        if(r_max<R){
            r_max+=2;
        }
        if(type == 1){MLV_update_window();}
    }
    if(type == 0){
        if(figure == 1){
            MLV_draw_circle(xc,yc,r_max,MLV_COLOR_RED);
        }
        else{
            MLV_draw_rectangle(xc-r_max,yc-r_max,r_max*2,r_max*2,MLV_COLOR_RED);
        }
        affiche_pnt(lst);
        affiche_conv(conv);
        MLV_update_window();
    }
    free_point(lst);
    if(i>=3){
        free_vtx(conv->pol);
    }
}


/*########################################################################
                FONCTIONS POUR LES CHOIX DE L'UTILISATEUR
########################################################################*/



void choix_affichage(int *type, int *jouer){
    MLV_clear_window(MLV_COLOR_WHITE);
    int x;
    int y;
    MLV_draw_text_box(250,150,500,250,"GRAPHIQUE",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_draw_text_box(250,550,500,250,"TERMINAL",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_draw_text_box(1,1,100,50,"QUITTER",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_update_window();
    MLV_wait_mouse(&x,&y);
    if(x>250 && x<750 && y>150 && y<400){
        *type = 1;
        MLV_clear_window(MLV_COLOR_WHITE);
        MLV_draw_text(300,500,"Le choix du nombre de point se fera dans le terinal.",MLV_COLOR_RED);
        MLV_update_window();
        return ;
    }
    else{
        if(x>250 && x<750 && y>550 && y<800){
            *type = 0;
            MLV_clear_window(MLV_COLOR_WHITE);
            MLV_draw_text(300,500,"Le choix du nombre de point se fera dans le terinal.",MLV_COLOR_RED);
            MLV_update_window();
            return ;
        }
        else{
            if(x>1 && x<101 && y>1 && y<51){
                *jouer = 0;
                return ;
            }
            else{
                choix(type, jouer);
            }
        }
    }
}



void choix_fig(int *figure, int *jouer){
    MLV_clear_window(MLV_COLOR_WHITE);
    int x;
    int y;
    MLV_draw_text_box(250,150,500,250,"CERCLE",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_draw_text_box(250,550,500,250,"CARRE",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_draw_text_box(1,1,100,50,"QUITTER",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_update_window();
    MLV_wait_mouse(&x,&y);
    if(x>250 && x<750 && y>150 && y<400){
        *figure = 1;
        return ;
    }
    else{
        if(x>250 && x<750 && y>550 && y<800){
            *figure = 0;
            return ;
        }
        else{
            if(x>1 && x<101 && y>1 && y<51){
                *jouer = 0;
                return ;
            }
            else{
                choix(figure, jouer);
            }
        }
    }
}



void choix(int *chox, int *jouer){
    MLV_clear_window(MLV_COLOR_WHITE);
    int x;
    int y;
    MLV_draw_text_box(250,150,500,250,"MANUEL",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_draw_text_box(250,550,500,250,"AUTOMATIQUE",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_draw_text_box(1,1,100,50,"QUITTER",5,MLV_COLOR_BLACK,MLV_COLOR_BLACK,MLV_COLOR_WHITE,MLV_TEXT_CENTER,MLV_HORIZONTAL_CENTER,MLV_VERTICAL_CENTER);
    MLV_update_window();
    MLV_wait_mouse(&x,&y);
    if(x>250 && x<750 && y>150 && y<400){
        *chox = 1;
    }
    else{
        if(x>250 && x<750 && y>550 && y<800){
            *chox = 0;
            return ;
        }
        else{
            if(x>1 && x<101 && y>1 && y<51){
                *jouer = 0;
                *chox = 4;
            }
            else{
                choix(chox, jouer);
            }
        }
    }

}

void NBR(int *NB_pnt) {
    char input[100];
    printf("Entrez un nombre : ");
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d", NB_pnt) != 1) {
        printf("Erreur : vous n'avez pas saisi un nombre valide.\n");
        NBR(NB_pnt);
    } 
    else {
        if(*NB_pnt<3){
            printf("mauvais chiffre\n");
            NBR(NB_pnt);
        }
        return ;
    }
}



int main(){
    int figure;
    int type;
    int jouer = 1;
    int chix = 0;
    int NB_pnt = 0;
    MLV_create_window("Convexe","convexe",Taille_x,Taille_y);
    while(jouer){
        choix(&chix,&jouer);
        if (chix==1){
            phase1(&jouer);
        }
        else{
            if (chix==0){
                choix_fig(&figure,&jouer);
                choix_affichage(&type, &jouer);
                NBR(&NB_pnt);
                graphique(type, figure, NB_pnt);
                MLV_wait_seconds(3);
            }
        }
    }
    MLV_free_window();
    return 0;
}