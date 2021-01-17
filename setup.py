from setuptools import setup, Extension

setup(
    name='Kelvandor',
    version='0.1.0',
    description='Node engine',
    url='https://github.com/richardjs/kelvandor',
    author='Richard Schneider',
    author_email='richard@schneiderbox.net',

    package_dir={'': 'src',},
    packages=['kelvandor',],

    # ext_modules=[
    #     Extension('kelvandor.c', sources=['src/kelvandor/c/c.c',],),
    # ],
)
