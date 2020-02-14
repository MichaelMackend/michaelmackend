from flask import Flask
app = Flask(__name__)


@app.route('/')
def index():
    return 'Hello, Index!'


@app.route('/world')
def hello_world():
    return 'Hello, WORLD!'


@app.route('/user/<username>')
def show_user_profile(username):
    # show the user profile for that user
    return 'User %s' % username


@app.route('/post/<int:post_id>')
def show_post(post_id):
    # show the post with the given id, the id is an integer
    return 'Post %d' % post_id


@app.route('/path/<path:subpath>')
def show_subpath(subpath):
    # show the subpath after /path/
    return 'Subpath %s' % subpath


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=1337)
