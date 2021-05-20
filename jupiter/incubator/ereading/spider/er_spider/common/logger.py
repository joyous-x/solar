import logging
import sys
import os

# logging.basicConfig(
#     level = logging.DEBUG,
#     stream = sys.stdout,
#     format = '%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s: %(message)s')

def currFileDir():
    current_path = os.path.abspath(__file__)
    parentPath = os.path.abspath(os.path.dirname(current_path) + os.path.sep + ".")
    return parentPath

def ensureDirExist(path):
    parentPath = os.path.abspath(os.path.dirname(path) + os.path.sep + ".")
    if os.path.exists(parentPath):
        return True
    else:
        logging.debug("ensureDirExist: ready to make dir %s" % parentPath)
    return os.makedirs(parentPath)

def makeDefaultLogger(logFilePath):
    logger = logging.getLogger("default")
    level = logging.DEBUG
    formatter = logging.Formatter("%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s: %(message)s")

    if len(logFilePath) > 0:
        fh = logging.FileHandler(logFilePath, mode='w')
        fh.setLevel(level)
        fh.setFormatter(formatter)
        logger.addHandler(fh)

    ch = logging.StreamHandler(stream=sys.stdout)
    ch.setLevel(level)
    ch.setFormatter(formatter)
    logger.addHandler(ch)

    return logger

logfile = os.path.join(os.path.dirname(os.path.dirname(currFileDir())), 'log/logger.log')
ensureDirExist(logfile)
logger = makeDefaultLogger(logfile)