# coding=utf-8
import os
from flask import redirect
from pymars.portal_flask.flaskapp import FlaskApp
import subprocess
from multiprocessing import Pool


app_configs = {
    "root_path": os.path.dirname(os.path.abspath(__file__)),
    "static_folders": {
        "static": "./server/static",
        "home": "/Users/jiao/Workspace/solar/mercury/package/myapp/home",
        "blog": "/Users/jiao/Workspace/solar/mercury/package/myapp/blog"
    },
    "template_folder": "./server/template"
}

srv = FlaskApp(configs=app_configs)
srv.register_plugin(plugin_module_path="pymars.portal_flask.plugins.test")
srv.register_plugin(plugin_module_path="pymars.portal_flask.plugins.ai")
srv.register_all_plugins()
app = srv.get_app()


@app.route('/')
def hello_world():
    return 'Hello, World!'


@app.route('/favicon.ico')
def favicon():
    return redirect('/static/favicon.ico')


@app.route('/<string:filename>.html')
def page_index(filename):
    return redirect('/static/{}.html'.format(filename))


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
