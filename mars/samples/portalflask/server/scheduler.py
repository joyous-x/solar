# coding=utf-8
import os
import sys
import time
import asyncio
import uvloop
import schedule


os.environ['MODE'] = 'PRO'
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


class Scheduler(object):
    def __init__(self, configs: dict()) -> None:
        self._configs = configs
        asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
        self._loop = asyncio.get_event_loop()
        self._scheduler = schedule.Scheduler()

    def regist_task(self, interval: int, task: object, at=":00", unit="seconds"):
        """ regist_task
        if unit in ['days', 'hours', 'minutes'], we can specify a particular time that the job should be run at.

        - For daily jobs -> `HH:MM:SS` or `HH:MM`
        - For hourly jobs -> `MM:SS` or `:MM`
        - For minute jobs -> `:SS`

        e.g. `every().hour.at(':30')` vs. `every().minute.at(':30')`
        """
        if unit == "seconds":
            self._scheduler.every(interval).seconds.do(task)
        elif unit == "minutes":
            job = self._scheduler.every(interval).minutes
            if at is not None and isinstance(at, str) and len(at) > 0:
                job = job.at(at)
            job.do(task)
        elif unit == "hours":
            job = self._scheduler.every(interval).hours
            if at is not None and isinstance(at, str) and len(at) > 0:
                job = job.at(at)
            job.do(task)
        elif unit == "days":
            job = self._scheduler.every(interval).days
            if at is not None and isinstance(at, str) and len(at) > 0:
                job = job.at(at)
            job.do(task)
        else:
            raise ValueError("invalid unit:{}".format(unit))

    def run_scheduler(self, interval=1):
        """
        :param interval: interval seconds for checking schedule.run_pending()
        :return:
        """
        while True:
            self._scheduler.run_pending()
            time.sleep(interval)

    def run_in_loop(self, function: object):
        # asyncio.ensure_future(coroutine) 和 loop.create_task(coroutine)都可以创建一个task
        task = asyncio.ensure_future(function(self._loop))
        self._loop.run_until_complete(task)
        return task.result() or None

    def run(self):
        """
        run scheduler in aysncio
        """
        # TODO:

