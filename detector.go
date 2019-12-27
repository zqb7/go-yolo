package main

/*
#cgo CFLAGS: -I .
#cgo LDFLAGS: -L . -ldarknet
# include "darknet.h"
# include <stdio.h>
# include "string.h"
# include <stdlib.h>
# include "go-yolo.h"
*/
import "C"
import "fmt"

func main() {
    Ready(
        "./yolov3.cfg",  // cfgfile
        "../yolov3.weights", // weightfile
        "./coco.data",  // coco.data
    )
    var l C.LinkList
    // l = ImageDetector("./dog.jpg",0.5)
    l = Detector("./dog.jpg")
    result := make([]R,0)
    if l.next != nil {
        l = l.next
        for {
            bbox := box{
                X: float64(l.data.bbox.x),
                Y: float64(l.data.bbox.y),
                W: float64(l.data.bbox.w),
                H: float64(l.data.bbox.h),
            }
            result = append(result,R{
                Name: PrintName(l.data.name),
                Prob: float64(l.data.prob),
                Box: bbox,
            })
            if l.next == nil {
                break
            }else {
                l = l.next
            }
        }
    }
    fmt.Println(result)

}
func Detector(imgPath string) C.LinkList {
    return ImageDetector(imgPath, 0.5)
}

func PrintName(data interface{}) string {
    var name = ""
    switch v := data.(type) {
    case [30]C.char:
        for i:=0;i<len(v);i++ {
            if v[i] <= 0 {
                return name
            }
            name += string(v[i])
        }
        return name
    default:
        return name
    }
    return name
}

func ImageDetector(fn string, t float64) C.LinkList {
    return C.image_detector(
        C.CString(fn),
        C.float(t),
        )
}

// 加载模型
func Ready(cfgFile,weightFile, metaFileName string)  {
    C.ready(
        C.CString(cfgFile),
        C.CString(weightFile),
        C.CString(metaFileName),
    )
}

type R struct {
    Name string
    Prob float64
    Box  box
}

type box struct {
    X float64
    Y float64
    W float64
    H float64
}
