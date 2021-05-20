package main

import (
	"context"
	"fmt"
	"sync"
	"time"
)

// RoutineWithTimeout my go routine
type RoutineWithTimeout struct {
	TimeoutSec int
	chDone     chan int
}

// Run ...
func (r *RoutineWithTimeout) Run(callback func()) {
	if r.TimeoutSec < 1 {
		r.TimeoutSec = 10
	}
	r.chDone = make(chan int)
	timer := time.NewTimer(time.Duration(time.Duration(r.TimeoutSec) * time.Second))
	defer timer.Stop()

	go r.realDO()

	select {
	case <-timer.C:
		fmt.Printf("(timeout)this is a time : %s \n", time.Now().Format("2006-01-02 15:04:05"))
	case <-r.chDone:
		fmt.Printf("(done)this is a time : %s \n", time.Now().Format("2006-01-02 15:04:05"))
	}

	if nil != callback {
		callback()
	}
}

func (r *RoutineWithTimeout) realDO() {
	for i := 0; i >= 0; i++ {
		fmt.Printf("currint id = %d \n", i)
		time.Sleep(time.Duration(time.Second))
	}
	r.chDone <- 1
}

func useChan() {
	waitGroup := &sync.WaitGroup{}
	maxRoutines := 10

	callback := func() {
		waitGroup.Done()
		fmt.Printf("callback done\n")
	}
	for i := 0; i < maxRoutines; i++ {
		waitGroup.Add(1)
		go new(RoutineWithTimeout).Run(callback)
	}

	waitGroup.Wait()
}

func useContext() {
	// ctx, cancel := context.WithDeadline(context.Background(), time.Now().Add(time.Second*10))
	// maxRoutines := 10
	testCh := make(chan int)
	waitGroup := &sync.WaitGroup{}

	myprint := func() {
		timer := time.NewTicker(time.Second)
		defer timer.Stop()
		for {
			select {
			case t := <-timer.C:
				fmt.Printf("current is : %s %v \n", time.Now().Format("2016-01-02 15:04:05"), t)
			case <-testCh:
				fmt.Printf("testch is hit \n")
				waitGroup.Done()
				goto END
			}
		}
	END:
		fmt.Printf("my print end \n")
	}

	waitGroup.Add(1)
	go myprint()
	waitGroup.Add(1)
	go myprint()

	time.Sleep(time.Second * 3)
	close(testCh)
	waitGroup.Wait()
	fmt.Printf("hello \n")
}

// RoutineTree a group of routines
type RoutineTree struct {
	ctx       context.Context
	fnCancel  context.CancelFunc
	waitGroup *sync.WaitGroup
}

// Init ...
func (r *RoutineTree) Init(timeout time.Duration) {
	r.ctx, r.fnCancel = context.WithTimeout(context.Background(), timeout)
	r.waitGroup = &sync.WaitGroup{}
}

// Wait ...
func (r *RoutineTree) Wait() {
	r.waitGroup.Wait()
}

func (r *RoutineTree) run(fn func(), chOut chan int) {
	defer func() {
		if err := recover(); err != nil {
			fmt.Println("RoutineTree run panic:", err)
		}
	}()
	fn()
	chOut <- 0
}

// NewRoutine ...
func (r *RoutineTree) NewRoutine(fn func()) {
	r.waitGroup.Add(1)
	go r.newRoutine(fn)
}

func (r *RoutineTree) newRoutine(fn func()) {
	if nil == r.waitGroup {
		go fn()
		return
	}

	chOut := make(chan int)
	go r.run(fn, chOut)

	select {
	case <-r.ctx.Done():
	case <-chOut:
	}
	r.waitGroup.Done()
}

func main() {
	realDo := func() {
		for i := 0; i >= 0; i++ {
			fmt.Printf("currint id = %d \n", i)
			time.Sleep(time.Duration(time.Second))
		}
	}

	tree := &RoutineTree{}
	tree.Init(time.Duration(time.Second * 10))
	tree.NewRoutine(realDo)
	tree.NewRoutine(realDo)
	tree.NewRoutine(realDo)
	tree.Wait()
}
