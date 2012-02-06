import sys, os, time

sys.path.append(os.path.abspath('tools/sphinxext'))

# templates
templates_path = ['templates']

# General configuration
extensions = [ 'sphinx.ext.refcounting', 'sphinx.ext.coverage', 'sphinx.ext.doctest']

# General substitutions
project = 'The Co Language Documentation'
copyright = 'Yecheng Fu'

# Options for HTML Output
# -----------------------

# theme
html_theme = "sphinxdoc"

# short title
html_short_title = "The Co Language Documentation"

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
html_last_updated_fmt = '%b %d, %Y'

# docs title
html_title = html_short_title

# Options for LaTex Output
# ------------------------

# The paper size ('letter' or 'a4')
latex_paper_size = 'a4'

# The font size ('10pt', '11pt' or '12pt)
latex_font_size = '10pt'

# Additional templates that should be rendered to pages
html_additional_pages = {
    'index': 'index.html',
}
