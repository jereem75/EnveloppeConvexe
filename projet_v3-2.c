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
}Point;

typedef struct vrtx{ 
Point *s;
struct vrtx *prev; 
struct vrtx *next; 
}Vertex, *Polygon;

typedef struct conv{
Polygon pol;
struct conv *suivant;
int curlen;
int maxlen;
}Convexhull;


/*fonction qui prend en parametre 3 points et qui renvoie 1 si 
C est à droite de A-B, 0 sinon*/
int position_pnt(Point *A, Point *B, Point *C);

/*fonction  qui prend en parametres deux entiers x,y et qui 
alloue le point*/
Point *alloue_pnt(int x, int y);

/*fonction qui prend en paramètre les coordonnées x et y d'un 
point et crée un nouveau vertex qui contient ce point*/
Polygon allouer_vtx(int x, int y);

/*fonction qui prend en parametre un vertex de type Polygon et qui le libere*/
void free_vtx(Polygon vtx);

/*fonction qui prend en parametre un pointeur vers une structure Convexehull 
et qui balaye vers l’avant et supprime un vtx du convexe si il en fait plus 
partie et appelle récursivement la foncion gere_lst_conv avec le vertex supprimé*/
void netoy_avant(Convexhull *conv);

/*fonction qui prend en parametre un pointeur vers une structure Convexehull 
et qui balaye vers l’arrière et supprime un vtx du convexe si il en fait plus 
partie et appelle récursivement la foncion gere_lst_conv avec le vertex supprimé*/
void netoy_arriere(Convexhull *conv);

/*fonction prend en paramètre une liste chainee de convexehull et un vertex 
new et essaie d'insérer ce vertex dans un des convexhull de la liste en respectant 
les regles d'insertion dans un convexe*/
int gere_pnt(Convexhull* lst, Polygon new);

/*fonction qui prend en parametre un convexe deux couleurs choisies prealablement 
pour l'interieur du convexe et pour les lignes et qui dessine graphiquement le convexe*/
void affiche_conv(Convexhull* conv, MLV_Color color, MLV_Color ligne);

/*fonction qui prend en parametre un entier figure representant la figure choisie 
et un entier r representant le rayon et qui génere aléatoirement un point contenu 
dans un polygone selon un rayon et une figure donné*/
Polygon genere_pnt(int figure, int r);

/*fonction qui prend en parametre trois entiers representant respectivement le type 
d'affichage choisi ansi que la figure choisie et un nombre de points et qui affiche 
le convexe d'une figure donné (cercle ou carré) selon un type d'affichage (graphique ou terminal)*/
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
choisi et une valeur permettant de savoir si le joueur veut continuer de jouer et qui affiche 
une interface graphique permettant à l'utilisateur de choisir entre afficher les résultats dans 
une fenêtre graphique ou dans le terminal.*/
void choix_affichage(int *type, int *jouer);

/*fonction qui prend en parametre deux entiers pointeurs representant le choix du joueur et une 
valeur permettant de savoir si le joueur veut continuer de jouer et qui affiche l'interface 
graphique permettant au joeur de choisir sa figure (cercle ou carre)*/
void choix_fig(int *figure, int *jouer);

/*fonction qui prend en parametre un entier pointeur representant un nombre de points et verifie 
si la saisie de l'utilisateur est bien un entier*/
void NBR(int *NB_pnt);

/*prend en paramètre un pointeur reprensentant la liste des convexes et un vertex et qui ajoute 
ce vertex à l'une des enveloppes convexes de la liste si cela est possible sinon elle crée une 
nouvelle enveloppe convexe avec ce vertex comme premier élément*/
void gere_lst_conv(Convexhull* lst, Polygon new);

/*fonction qui prend en parametre un Polygon et qui crée une nouvelle structure de type Convexhull 
contenant le polygone*/
Convexhull* allouer_conv(Polygon pol);

/*fonction qui prend en parametre la liste chainee des convexe et appelle la fonction affiche_conv 
en allouant une couleur respective a chaque convexe*/
void affiche_lst(Convexhull *lst);

/*fonction qui prend en parametre la liste chainee des convexe et libere chaque convexe*/
void free_lst(Convexhull *lst);


/*########################################################################
                            FONCTIONS POUR LE JEUX
########################################################################*/




void free_vtx(Polygon vtx){
    Polygon courant = vtx;
    while(courant!=vtx){
        Polygon tmp = courant->next;
        free(courant->s);
        free(courant);
        courant=tmp;
    }
}


void free_lst(Convexhull *lst){
    if(lst == NULL){
        return ;
    }
    Convexhull *tmp=lst->suivant;
    while(tmp){
        free_vtx(lst->pol);
        free(lst);
        lst=tmp;
        tmp=lst->suivant;
    }
    free(lst);
}

void affiche_conv(Convexhull* conv, MLV_Color color, MLV_Color ligne){
    if(conv->curlen == 1){
        MLV_draw_filled_circle(conv->pol->s->x,conv->pol->s->y,5,MLV_COLOR_RED);
        return ;
    }
    if(conv->curlen == 2){
        MLV_draw_filled_circle(conv->pol->s->x,conv->pol->s->y,5,MLV_COLOR_RED);
        MLV_draw_filled_circle(conv->pol->next->s->x,conv->pol->next->s->y,5,MLV_COLOR_RED);
        return ;
    }
    int tab_x[conv->curlen];
    int tab_y[conv->curlen];
    Polygon pol=conv->pol;
    for(int i = 0; i<conv->curlen; i++){
        tab_x[i] = pol->s->x;
        tab_y[i] = pol->s->y;
        MLV_draw_filled_circle(pol->s->x,pol->s->y,5,ligne);
        pol = pol->next;
    }
    MLV_draw_filled_polygon(tab_x, tab_y, conv->curlen, color);
    MLV_draw_polygon(tab_x, tab_y, conv->curlen, ligne);
}


void affiche_lst(Convexhull *lst){
    int i = 0;
    MLV_Color color;
    MLV_Color ligne;
    for(;lst;lst = lst->suivant){
        i++;
        if ( i == 1){
            color = MLV_rgba(255, 0, 255,30);
            ligne = MLV_rgba(255, 0, 255,255);
        }
        if( i == 2){
            color = MLV_rgba(0, 255, 255,30);
            ligne = MLV_rgba(0, 255, 255,255);
        }
        if ( i == 3){
            color = MLV_rgba(0, 255, 0,30);
            ligne = MLV_rgba(0, 255, 0,255);
        }
        if( i == 4){
            color = MLV_rgba(127, 0, 255,30);
            ligne = MLV_rgba(127, 0, 255,255);
            i = 0;
        }
        affiche_conv(lst,color, ligne);
    }
}


Point *alloue_pnt(int x, int y){
    Point *new = (Point*)malloc(sizeof(Point));
    if(new == NULL){
        return NULL;
    }
    new->x=x;
    new->y=y;
    return new;
}


Polygon allouer_vtx(int x, int y){
    Point *pnt=alloue_pnt(x, y);
    if (pnt == NULL){
        return NULL;
    }
    Polygon new = (Vertex*)malloc(sizeof(Vertex));
    if (new==NULL){
        return NULL;
    }
    new->s=pnt;
    new->next=new;
    new->prev=new;
    return new;
}

Convexhull* allouer_conv(Polygon pol){
    Convexhull* new = (Convexhull*)malloc(sizeof(Convexhull));
    if(new == NULL){
        return NULL;
    }
    new->pol = pol;
    new->curlen = 1;
    new->maxlen = 1;
    new->suivant = NULL;
    return new;
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
        B->next = B;
        B->prev=B;
        gere_lst_conv(conv, B);
        B = C;
        C = C->next;
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
        B->next = B;
        B->prev=B;
        gere_lst_conv(conv, B);
        B = C;
        C = C->prev;
        conv->curlen-=1;
    }
}



int gere_pnt(Convexhull* lst, Polygon new){
    if(lst->curlen==1){
        lst->pol->next = new;
        lst->pol->prev = new;
        new->next = lst->pol;
        new->prev = lst->pol;
        lst->curlen += 1;
        return 1;
    }
    Point *C = new->s;
    Polygon depart = lst->pol;
    Polygon A = lst->pol;
    Polygon B = A->next;
    for(;B!=depart;){
        if(position_pnt(A->s,B->s,C)==0){
            A->next=new;
            new->prev=A;
            new->next=B;
            B->prev=new;
            lst->curlen+=1;
            lst->pol=new;
            netoy_arriere(lst);
            netoy_avant(lst);
            if(lst->maxlen < lst->curlen){
                lst->maxlen = lst->curlen;
            }
            return 1;
        }
        A=B;
        B=B->next;
    }
    if(position_pnt(A->s,B->s,C)==0){
        A->next=new;
        new->prev=A;
        new->next=B;
        B->prev=new;
        lst->curlen+=1;
        lst->pol=new;
        netoy_arriere(lst);
        netoy_avant(lst);
        if(lst->maxlen < lst->curlen){
            lst->maxlen = lst->curlen;
        }
        return 1;
    }
    return 0;
}

void gere_lst_conv(Convexhull* lst, Polygon new){
    if(lst == NULL){
        lst = allouer_conv(new);
        return ;
    }
    if(lst->curlen==1){
        lst->pol->next = new;
        lst->pol->prev = new;
        new->next = lst->pol;
        new->prev = lst->pol;
        lst->curlen += 1;
        return ;
    }
    for(;lst->suivant; lst = lst->suivant){
        if(gere_pnt(lst,new) == 1){
            return ;
        }
    }
    if(gere_pnt(lst,new) == 1){
        return ;
    }
    lst->suivant = allouer_conv(new);
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
            break;
        }
        if(x>800 && x<1000 && y>1 && y<51){
            phase1(jouer);
            break;
        }
        if(x>450 && x<550 && y>1 && y<51){
            break;
        }
        Polygon new = allouer_vtx(x, y);
        if(i==1){
            conv = allouer_conv(new);
        }
        else{
            gere_lst_conv(conv, new);
        }
        affiche_lst(conv);
        MLV_update_window();
    }
    free_lst(conv);
}


Polygon genere_pnt(int figure, int r){
    int x = xc-r + rand()%(2*r);
    int y = yc-r + rand()%(2*r);
    if (figure == 1){
        while(sqrt((x - xc)*(x - xc) + (y - yc) * (y - yc))>r){
            x = xc-r + rand()%(2*r);
            y = yc-r + rand()%(2*r);
        }
    }
    Polygon new = allouer_vtx(x, y);
    return new;
}


void graphique(int type, int figure, int NB_pnt){
    MLV_clear_window(MLV_COLOR_WHITE);
    Convexhull *conv = NULL;

    int i = 0;
    double r_max = 5;
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
        Polygon new = genere_pnt(figure, r_max);
        if(i==1){
            conv = allouer_conv(new);
        }
        else{
            gere_lst_conv(conv, new);
        }
        if(r_max<R){
            if((double)((double)R/(double)NB_pnt)<1){
                r_max+= ((double)((double)R/(double)NB_pnt)+0.2);
            }
            else{
                r_max+= ((double)((double)R/(double)NB_pnt));
            }
        }
        if(type == 1){
            affiche_lst(conv);
            MLV_update_window();
        }
    }
    if(type == 0){
        if(figure == 1){
            MLV_draw_circle(xc,yc,r_max,MLV_COLOR_RED);
        }
        else{
            MLV_draw_rectangle(xc-r_max,yc-r_max,r_max*2,r_max*2,MLV_COLOR_RED);
        }
        affiche_lst(conv);
        MLV_update_window();
    }
    free_lst(conv);
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