
import os
from distutils import dir_util
import pytest


@pytest.fixture
def test_data_dir(tmpdir, request):
    filename = request.module.__file__
    root_dir = os.path.dirname(filename)
    input_dir = os.path.join(root_dir, "../../", "mmtf_spec")
    dir_util.copy_tree(input_dir, os.path.join(tmpdir.strpath, "mmtf_spec"))
    input_dir = os.path.join(root_dir, "../../", "temporary_test_data")
    dir_util.copy_tree(input_dir, os.path.join(tmpdir.strpath, "temporary_test_data"))
    return tmpdir
