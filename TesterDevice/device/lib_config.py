import sys
import os

module_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'cmake-build-debug'))

sys.path.append(module_path)

import TesterLib