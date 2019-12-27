# include <stdio.h>
# include "string.h"
# include "darknet.h"
# include <stdlib.h>

typedef struct{
    char name[30];
    float prob;
    box bbox;
}TestResult;

typedef struct Node {
    TestResult data;
    struct Node * next;
}Node, *LinkList;


network *net;
metadata meta;
void ready(char *cfgfile, char *weightfile, char *metafilename) {
    net = load_network(cfgfile, weightfile,0);
    meta = get_metadata(metafilename);
}
//初始化单链表
void InitList(LinkList *L) {
    *L = (LinkList)malloc(sizeof(Node)); //建立头结点
    (*L)->next = NULL; //建立空的单链表L

}

LinkList image_detector(char *imgname, float thresh)
{
    LinkList List;
    InitList(&List);

    image im = load_image_color(imgname,0,0);
    network_predict_image(net,im);

    int a,pnum = 0;
    detection *dets = get_network_boxes(net, im.w, im.h, thresh, .5, &a,0,&pnum);
    layer l = net->layers[net->n-1];
    // set_batch_network(net, 1);
    srand(2222222);
    float nms=.45;
    if (nms)
    {
        do_nms_obj(dets,13,meta.classes,nms);
        do_nms_sort(dets, l.side*l.side*l.n, l.classes, nms);
    }

    int i,j = 0;
    Node *s;
    for(j = 0; j < pnum; ++j){
        for (i = 0; i < meta.classes; i++)
        {
            if (dets[j].prob[i] > 0)
            {   
                box b = dets[j].bbox;
                // res[i].bbox = b;
                // strcpy(res[i].name,meta.names[i]);
                // res[i].prob = dets[j].prob[i];

                TestResult body;
                strcpy(body.name,meta.names[i]);
                body.bbox = b;
                body.prob = dets[j].prob[i];
                s = (Node *)malloc(sizeof(Node));
                s->data = body;
                s->next = List->next;
                List->next = s;
            }
            
        }
    }

    free_image(im);
    free_detections(dets,0);
    return List;
};