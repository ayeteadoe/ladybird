# Copyright (c) 2025, Tim Flynn <trflynn89@ladybird.org>
# Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import signal
import subprocess
import sys

from pathlib import Path
from typing import Optional
from typing import Union


def ensure_ladybird_source_dir() -> Path:
    ladybird_source_dir = os.environ.get("LADYBIRD_SOURCE_DIR", None)
    ladybird_source_dir = Path(ladybird_source_dir) if ladybird_source_dir else None

    if not ladybird_source_dir or not ladybird_source_dir.is_dir():
        root_dir = run_command(["git", "rev-parse", "--show-toplevel"], return_output=True, exit_on_failure=True)
        assert root_dir

        os.environ["LADYBIRD_SOURCE_DIR"] = root_dir
        ladybird_source_dir = Path(root_dir)

    return ladybird_source_dir


def run_command(
    command: list[str],
    input: Union[str, None] = None,
    return_output: bool = False,
    exit_on_failure: bool = False,
    cwd: Union[Path, None] = None,
) -> Optional[str]:
    stdin = subprocess.PIPE if type(input) is str else None
    stdout = subprocess.PIPE if return_output else None

    try:
        # FIXME: For Windows, set the working directory so DLLs are found.
        with subprocess.Popen(command, stdin=stdin, stdout=stdout, text=True, cwd=cwd) as process:
            (output, _) = process.communicate(input=input)

            if process.returncode != 0:
                if exit_on_failure:
                    sys.exit(process.returncode)
                return None

    except KeyboardInterrupt:
        process.send_signal(signal.SIGINT)
        process.wait()

        sys.exit(process.returncode)

    if return_output:
        return output.strip()

    return None
