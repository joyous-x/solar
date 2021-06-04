# coding=utf-8
import os
import shutil
import random
import json
import requests
from pymars.utils.document import *


class MyChecker(object):
    def __init__(self, filepath, passwd=None):
        self.filepath, self.passwd = filepath, passwd
        self.plumber = PdfPlumberWrapper(filepath, passwd)

    @staticmethod
    def walk_dir(file_dir, file_name):
        files = []
        if file_name is not None:
            files.append(os.path.join(file_dir, file_name))
        else:
            for root, dirs, names in os.walk(file_dir, topdown=False):
                for sub_name in names:
                    files.append(os.path.join(root, sub_name))
                for sub_dir in dirs:
                    for ss_root, _, ss_files in os.walk(os.path.join(root, sub_dir), topdown=False):
                        for ss_name in files:
                            files.append(os.path.join(ss_root, ss_name))
        return files

    def parse_docx(self, docx_path, output_dir="."):
        texts, images, tables = DocxWrapper.extract_texts_tables(docx_path)
        images = DocxWrapper.extract_images(docx_path, output_dir, "./tmp")
        return texts, images, tables

    def parse_xls(self):
        templateDir = os.path.dirname(os.path.abspath(__file__))
        wb = load_workbook(os.path.join(templateDir, "bill_template.xlsx"))
        shPreface = wb.get_sheet_by_name(wb.sheetnames[0])
        shDetail = wb.get_sheet_by_name(wb.sheetnames[1])

        texts, grids = None, None
        pass
        return texts, grids

    @staticmethod
    def record_page_info(filepath, pageno, texts, images, tables, output_dir_root, do_record=True):
        file_dir, file_name = os.path.split(filepath)
        file_name, file_suffix = os.path.splitext(file_name)

        text_sample, images_count, table_count, text_count = "", 0, 0, 0
        if texts:
            text_count += len(texts.replace(' ', ''))
            text_sample = texts[:min(text_count, 30)]
        images_count += len(images) if images else 0
        table_count += len(tables) if tables else 0
        output_dir_name = "{}{}.{}.{}_{}_{}".format(file_name, file_suffix, pageno, images_count, table_count, text_count)
        output_dir_path = os.path.join(output_dir_root, output_dir_name)

        if do_record:
            shutil.rmtree(output_dir_path, ignore_errors=True)
            os.makedirs(output_dir_path, exist_ok=True)

            with open(os.path.join(output_dir_path, "texts.txt"), "a+", encoding='utf-8') as f:
                f.write(texts.replace(' ', ''))
            if images:
                PdfPlumberWrapper.record_images(images, os.path.join(output_dir_path, "images"))
            if tables:
                CamelotWrapper.record_tables(tables, output_dir_path)
        print("\t page({}) has {} images {} tables, texts: {} ".format(pageno, images_count, table_count, repr(text_sample)))
        return text_count, images_count, table_count

    @staticmethod
    def extract_pdf(file_path, target_dir, prefix=""):
        text_count, images_count, table_count = 0, 0, 0
        pages_count = CamelotWrapper.pages_count(file_path)
        if pages_count is None or pages_count > 100:
            return pages_count if pages_count else -1, -1, -1, -1
        print("==> prepare: {} , total pages: {}".format(file_path, pages_count))
        checker = MyChecker(file_path, None)
        for index in range(1, 1+pages_count):
            _text, _table, _images = checker.parse_pdf({index}, writer=MyChecker.record_page_info, output_dir_root=target_dir)
            text_count += len(_text[index]) if _text[index] else 0
            images_count += len(_images[index]) if _images[index] else 0
            table_count += len(_table[index]) if _table[index] else 0
        print("===> {} has {} words {} images {} tables".format(file_path, text_count, images_count, table_count))
        return pages_count, text_count, images_count, table_count

    def parse_pdf(self, pages: set() = None, writer=None, output_dir_root=None):
        """ 获取 pdf 中的 texts, images, tables
        注意：为了优化 tables 的检查时间较长的问题，先判断是否有文本，如果没有文本检出，则认为没有表格
        """
        texts, images, tables = dict(), dict(), dict()
        for page_number in pages:
            texts[page_number], images[page_number], tables[page_number] = None, None, None
            page_data = self.plumber.parse_pdf({page_number})
            for _, texts_images in page_data.items():
                texts[page_number] = texts_images[0]
                images[page_number] = texts_images[1]
            if page_number in texts and texts[page_number] and len(texts[page_number]) > 0:
                pdf_tables = CamelotWrapper(self.filepath, self.passwd, pages=str(page_number))
                tables[page_number] = pdf_tables.parse_tables({page_number})
            else:
                tables[page_number] = None
            if writer is not None:
                writer(self.filepath, page_number, texts[page_number], images[page_number], tables[page_number], output_dir_root, do_record=False)
        return texts, tables, images


def __run_split_pages(pdf_dir, pdf_type, output_dir, name_prefix="", pages={1,2,3}):
    files = MyChecker.walk_dir(os.path.join(pdf_dir, pdf_type), None)
    for file_path in files:
        try:
            result = CamelotWrapper.split_pages(file_path, pages, output_dir, output_prefix=name_prefix)
            if result:
                target_dir = os.path.join(os.path.dirname(pdf_dir), "processed", pdf_type)
                if not os.path.exists(target_dir) or not os.path.isdir(target_dir):
                    os.makedirs(target_dir, exist_ok=True)
                _, file_name = os.path.split(file_path)
                shutil.move(file_path, os.path.join(target_dir, file_name))
        except Exception as e:
            print("split pages {} error: {}".format(file_path, e))


def __run_extract_data(pdf_dir, pdf_type, output_dir):
    files = MyChecker.walk_dir(os.path.join(pdf_dir, pdf_type), None)
    for file_path in files:
        result = MyChecker.extract_pdf(file_path, output_dir, "word_")
        if result:
            page_count, text_count, images_count, table_count = result
            statistics_file = os.path.join(os.path.dirname(pdf_dir), pdf_type + ".txt")
            with open(statistics_file, "a+") as f:
                f.write("{}, {}, {}, {}, {} \n".format(file_path, page_count, text_count, images_count, table_count))
            target_dir = os.path.join(os.path.dirname(pdf_dir), "processed", pdf_type)
            if not os.path.exists(target_dir) or not os.path.isdir(target_dir):
                os.makedirs(target_dir, exist_ok=True)
            _, file_name = os.path.split(file_path)
            shutil.move(file_path, os.path.join(target_dir, file_name))


class CheckUnhandled(object):
    def __init__(self, ):
        pass

    def check(self, filepath_raw, filepath_handled, unhandled_dir):
        raw_filenames = os.listdir(filepath_raw)
        raw_filenames = filter(lambda i: os.path.isfile(os.path.join(filepath_raw, i)), raw_filenames)

        handled_name_set = set()
        handled_filenames = os.listdir(filepath_handled)
        for name in handled_filenames:
            if "(1)" in name:
                os.rename(os.path.join(filepath_handled, name), os.path.join(filepath_handled, "__"+name))
                continue
            n,s = os.path.splitext(name)
            handled_name_set.add(n)

        for name in raw_filenames:
            n, s = os.path.splitext(name)
            if n in handled_name_set:
                continue
            shutil.move(os.path.join(filepath_raw, name), unhandled_dir)

    def split_dataset(self, filepath_raw, filepath_handled):
        raw_filenames = os.listdir(filepath_raw)
        raw_filenames = list(filter(lambda i: os.path.isfile(os.path.join(filepath_raw, i)), raw_filenames))
        handled_filenames = os.listdir(filepath_handled)
        handled_filenames = list(filter(lambda i: os.path.isfile(os.path.join(filepath_handled, i)), handled_filenames))

        small_set = set()
        for name in raw_filenames:
            n, s = os.path.splitext(name)
            if random.random() < 0.03:
                small_set.add(n)

        for name in raw_filenames:
            n, s = os.path.splitext(name)
            if n in small_set:
                shutil.move(os.path.join(filepath_raw, name), os.path.join(filepath_raw, "small"))
            else:
                shutil.move(os.path.join(filepath_raw, name), os.path.join(filepath_raw, "big"))

        for name in handled_filenames:
            n, s = os.path.splitext(name)
            if n in small_set:
                shutil.move(os.path.join(filepath_handled, name), os.path.join(filepath_handled, "small"))
            else:
                shutil.move(os.path.join(filepath_handled, name), os.path.join(filepath_handled, "big"))

    def run(self, root_dir="D:\\Workspace\\datasets\\pdf_convert\\process"):
        self.check(root_dir + "\\splited\\small", root_dir + "\\abbyy.1\\small", root_dir + "\\splited\\1")
        # self.split_dataset("D:\\Workspace\\datasets\\pdf_convert\\process\\splited", "D:\\Workspace\\datasets\\pdf_convert\\process\\wps")


class CallAbbyy(object):
    def __init__(self, host="81.70.250.254", port=8080):
        self.host = host
        self.port = port

    def call(self, url_path="/c2doc", filepath="", subdir=""):
        raw_filenames = os.listdir(os.path.join(filepath, subdir))
        raw_filenames = list(filter(lambda i: os.path.isfile(os.path.join(filepath, subdir, i)), raw_filenames))

        url = "http://{}:{}{}".format(self.host, self.port, url_path)
        for filename in raw_filenames:
            name, suffix = os.path.splitext(filename)
            data = {"src": "{}/{}".format(subdir, filename), "dst": "{}_dst/{}{}".format(subdir, name,".docx"), "action": "docx"}
            headers = {
                "Content-Type": "application/json;charset=UTF-8"
            }
            resp = requests.post(url, data=json.dumps(data), headers=headers)
            if resp.status_code == 200:
                print("ooo {} ok".format(filename))
            else:
                print("ooo {} error: {}".format(filename, resp.status_code))

    def rename(self, root_dir, src_dir, dst_dir):
        raw_filenames = os.listdir(os.path.join(root_dir, src_dir))
        raw_filenames = list(filter(lambda i: os.path.isfile(os.path.join(root_dir, src_dir, i)), raw_filenames))
        for filename in raw_filenames:
            name, suffix = os.path.splitext(filename)
            new_name = "{}.{}{}".format(name, src_dir, suffix)
            shutil.copy(os.path.join(root_dir, src_dir, filename), os.path.join(root_dir, dst_dir, new_name))


if __name__ == "__main__":
    #CheckUnhandled().run()
    CallAbbyy().rename("D:\\Workspace\\pdf_convert\\dataset\\sample\\normal\\", "yes_yes", "abbyy")
    CallAbbyy(host="127.0.0.1").call(filepath="D:\\Workspace\\pdf_convert\\dataset\\sample\\normal\\", subdir="small")
    raw_path = "D:\\Workspace\\datasets\\pdf_convert\\raw"
    out_path = "D:\\Workspace\\datasets\\pdf_convert\\output"
    #__run_split_pages(raw_path, "pdf_to_ppt", out_path, name_prefix="ppt_", pages={1, 2, 3, 4, 5})
    #result = CamelotWrapper.split_pages(raw_path + "\\error_split.pdf", {1, 2, 3, 4, 5}, raw_path, output_prefix="")