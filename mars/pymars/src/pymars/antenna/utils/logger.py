# coding = utf-8
import sys
import logging
import logging.handlers
import datetime

"""
logging.basicConfig(
    level=logging.DEBUG,
    stream=sys.stdout,
    format="%(asctime)s %(levelname)s %(filename)s:L%(levelno)s %(message)s"
)
"""


def logger_default(log_level=logging.DEBUG, using_logfile=True, log_filename="mylibs.log", rotating=True):
    format = "%(asctime)s %(levelname)s %(filename)s:L%(levelno)s %(message)s"
    logger = logging.getLogger(log_filename)
    logger.setLevel(level=log_level)
    s_handler = logging.StreamHandler()
    s_handler.setLevel(log_level)
    s_handler.setFormatter(logging.Formatter(format))
    logger.addHandler(s_handler)
    if using_logfile:
        if rotating:
            f_handler = logging.handlers.TimedRotatingFileHandler(log_filename, when='midnight', interval=1, backupCount=7,
                                                                  atTime=datetime.time(0, 0, 0, 0))
        else:
            f_handler = logging.FileHandler(log_filename)
        f_handler.setLevel(log_level)
        f_handler.setFormatter(logging.Formatter(format))
        logger.addHandler(f_handler)
    return logger


mlog = logger_default(using_logfile=True, rotating=True)
