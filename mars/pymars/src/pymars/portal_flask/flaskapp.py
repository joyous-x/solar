# coding=utf-8
import os
from flask import Blueprint, Flask, request, render_template, send_from_directory, url_for
import pkgutil
import importlib


try:
    import flask_bootstrap
except ImportError as e:
    flask_bootstrap = None
    _global_bootstrap_available = False
    print(e.__class__.__name__ + ": " + str(e))
else:
    _global_bootstrap_available = True


class FlaskHelper(object):
    """ FlaskHelper
    help to create hooks and configure the flask app, .etc
    """

    @staticmethod
    def create_app(name=__name__, configs=dict(), use_bootstrap=False):
        """
        对 Flask 的设置，参考：https://dormousehole.readthedocs.io/en/latest/config.html
        """
        flask_config = dict(
            root_path=configs.get("root_path") or ".", # os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
            template_folder=configs.get("template_folder") or "templates",
            static_folder=None,  # 不使用自动创建的 endpoint： static，后续会主动注册
            static_url_path=None,
        )
        flask_app = Flask(name, **flask_config)
        flask_app.templates_auto_reload = True
        flask_app.secret_key = None
        flask_app.config.update(
            TESTING=False,
            SECRET_KEY=configs.get("secret_key") or None,
            TEMPLATES_AUTO_RELOAD=True,
        )
        if use_bootstrap and _global_bootstrap_available:
            flask_bootstrap.Bootstrap(flask_app)
        return flask_app

    @staticmethod
    def init_app(flask_app, static_folder, template_folder):
        @flask_app.before_request
        def before_request():
            print("before_request: {} {} {}".format(request.path, request.method.lower(), request.remote_addr))

        @flask_app.after_request
        def after_request(resp):
            print("after_request : {} {}".format(resp.status_code, request.path))
            return resp

        @flask_app.teardown_appcontext
        def teardown_db(err):
            print("teardown_db : {}".format(err))

        @flask_app.teardown_request
        def teardown_request(err):
            print("teardown_request : {}".format(err))
            return

        @flask_app.errorhandler(403)
        def page_forbidden(error):
            return render_template('{}/403.html'.format(template_folder)), 403

        @flask_app.errorhandler(404)
        def page_not_found(error):
            return render_template('{}/404.html'.format(template_folder)), 404

        @flask_app.route('/static/<path:filename>', endpoint="static")
        def serve_static(filename):
            return send_from_directory(static_folder, filename)

        @flask_app.template_filter()
        def reverse(args):
            return args[::-1]

        @flask_app.template_global()
        def template_global():
            # TODO:
            return True


class Plugin(object):
    def __init__(self, blueprint: Blueprint):
        self._blueprint = blueprint

    def blueprint(self):
        return self._blueprint


class FlaskApp(object):
    def __init__(self, configs: dict()):
        self._configs = configs
        self._app = FlaskHelper.create_app(configs=self._configs)
        FlaskHelper.init_app(self._app, self._configs.get("static_folder"), self._configs.get("template_folder"))

    def get_config(self):
        """
        :return: flask configs
        """
        return self._configs

    def get_app(self):
        """
        :return: flask application object
        """
        return self._app

    def _register_plugin(self, plugin_module_path="plugins.test.view"):
        try:
            plugin_module = importlib.import_module(plugin_module_path)
        except ImportError as e:
            print("! register_plugin error : {}".format(e))
            return None

        for obj_name in ["blueprint", "sub_app"]:
            if not hasattr(plugin_module, obj_name):
                continue
            obj = getattr(plugin_module, obj_name)
            if isinstance(obj, Blueprint):
                self.get_app().register_blueprint(obj)
                print("* register_plugin registered(Blueprint) {}.{}".format(plugin_module.__name__, obj_name))
            elif isinstance(obj, Plugin):
                self.get_app().register_blueprint(obj.blueprint)
                print("* register_plugin registered(Plugin) {}.{}".format(plugin_module.__name__, obj_name))

        for func_name in dir(plugin_module):
            if func_name in ["init_plugin", ]:
                print("* register_plugin ready to execute {}.{}".format(plugin_module.__name__, func_name))
                getattr(plugin_module, func_name)(self)

    def register_plugin(self, plugin_module_path="plugins.test"):
        for v in ["view", "api"]:
            self._register_plugin(plugin_module_path + "." + v)
        return True

    def register_all_plugins(self, plugins="plugins"):
        try:
            plugins_module = importlib.import_module(plugins)
        except ImportError as e:
            print("register_all_plugins error : {}".format(e))
            return None
        for file_finder, name, is_pkg in pkgutil.iter_modules(plugins_module.__path__, plugins_module.__name__ + "."):
            print("{0} name: {1:12}, is_sub_package: {2}".format(file_finder, name, is_pkg))
            if is_pkg:
                self.register_plugin(name)
        return True
