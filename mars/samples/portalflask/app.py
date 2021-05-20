# coding=utf-8
from flask import redirect
from pymars.portal_flask.flaskapp import FlaskApp


srv = FlaskApp(configs={"root_path":".", "static_folder": "./server/static", "template_folder": "./server/template"})
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
