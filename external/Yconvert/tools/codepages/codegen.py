# -*- coding: utf-8 -*-
# ****************************************************************************
# Copyright © 2013 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2013-10-22.
#
# This file is distributed under the Simplified BSD License.
# License text is included with the source distribution.
# ****************************************************************************


class Error(Exception):
    pass


def _tokenize(tmpl_str):
    for line in tmpl_str.split("\n"):
        start = 0
        while True:
            end = line.find("[[[", start)
            if end == -1:
                if start != len(line):
                    yield "TEXT", line[start:]
                yield "NEWLINE", ""
                break
            yield "TEXT", line[start:end]
            start = end + 3
            end = line.find("]]]", start)
            if end == -1:
                raise Error("[[[ not followed by ]]] on the same line.")
            if line[start:].startswith("IF "):
                yield tuple(line[start:end].split(None, 2))
            elif line[start:].startswith("ELIF "):
                yield tuple(line[start:end].split(None, 2))
            elif line[start:end] in ("|", ">", "<"):
                yield line[start:end], ""
            elif line[start:].startswith("ELSE"):
                yield "ELSE", ""
            elif line[start:].startswith("ENDIF"):
                yield "ENDIF", ""
            elif line[start:].startswith("SET "):
                yield "SET", line[start + 4:end].lstrip()
            else:
                yield "EXPAND", line[start:end]
            start = end + 3


class Expander(object):
    def __init__(self, context=("", 0)):
        self.context = context

    def __call__(self, key, params, context):
        if callable(self.__getattribute__(key)):
            context = (self.context[0] + context[0],
                       self.context[1] + context[1])
            return self.__getattribute__(key)(params, context)
        else:
            return self.__getattribute__(key)

    def assign_value_(self, identifier, value):
        self.__setattr__(identifier, value)


class DictExpander(Expander):
    def __init__(self, dictionary):
        Expander.__init__(self)
        self._dict = dictionary

    def __call__(self, key, params, context):
        return self._dict[key]

    def assign_value_(self, identifier, value):
        self._dict[identifier] = value


class FuncExpander(Expander):
    def __init__(self, expand_func, assign_func=None):
        Expander.__init__(self)
        self._expand = expand_func
        self._assign = assign_func

    def __call__(self, key, params, context):
        return self._expand(key, params, context)

    def assign_value_(self, identifier, value):
        if self._assign:
            self._assign(identifier, value)


def is_empty_or_space(s):
    return not s or s.isspace()


class TemplateProcessor:
    def __init__(self, template, expander):
        self.template = template
        self.expander = expander
        self.lines = []
        self.cur_line = []
        self.column = 0
        self.scope = ["active"]
        self.line_no = 1
        self.alignments = []

    def __call__(self):
        ignore_newline = False
        try:
            for ttype, tstr in _tokenize(self.template):
                if ignore_newline:
                    ignore_newline = False
                    if ttype == "NEWLINE":
                        if is_empty_or_space("".join(self.cur_line)):
                            self.cur_line = []
                        self.line_no += 1
                        continue

                if ttype == "NEWLINE":
                    self.handle_NEWLINE()
                elif ttype == "IF":
                    self.handle_IF(tstr)
                    ignore_newline = is_empty_or_space("".join(self.cur_line))
                elif ttype == "ENDIF":
                    self.handle_ENDIF()
                    ignore_newline = is_empty_or_space("".join(self.cur_line))
                elif self.scope[-1] == "done":
                    pass
                elif ttype == "ELIF":
                    self.handle_ELIF(tstr)
                    ignore_newline = is_empty_or_space("".join(self.cur_line))
                elif ttype == "ELSE":
                    self.handle_ELSE()
                    ignore_newline = is_empty_or_space("".join(self.cur_line))
                elif self.scope[-1] == "inactive":
                    pass
                elif ttype == "TEXT":
                    self._add_text(tstr)
                elif ttype == "EXPAND":
                    ignore_newline = (not self.handle_EXPAND(tstr) and
                                      is_empty_or_space("".join(self.cur_line)))
                elif ttype == "SET":
                    self.handle_SET(tstr)
                    ignore_newline = is_empty_or_space("".join(self.cur_line))
                elif ttype == ">":
                    self.push_alignment()
                elif ttype == "|":
                    self.align()
                elif ttype == "<":
                    self.pop_alignment()
                else:
                    raise Error("Unknown token: " + ttype)
            if len(self.scope) != 1:
                raise Error("Number of IFs without closing ENDIFs: %d" %
                            (len(self.scope) - 1))
        except Error as ex:
            raise Error("[line %d]: %s" % (self.line_no, str(ex)))

    def handle_NEWLINE(self):
        self.line_no += 1
        if self.scope[-1] != "active":
            return
        self.lines.append("".join(self.cur_line).rstrip())
        self.cur_line = []
        self.column = 0

    def handle_EXPAND(self, key):
        if self.scope[-1] != "active":
            return False
        text = self._expand(key)
        if text is None:
            return False
        elif type(text) == str:
            if not text:
                return False
            self._add_text(text)
        elif type(text) == list:
            if not text:
                return False
            self._add_lines(text)
        else:
            self._add_text(str(text))
        return True

    def handle_IF(self, key):
        if self.scope[-1] != "active":
            self.scope.append("done")
        elif self._expand(key):
            self.scope.append("active")
        else:
            self.scope.append("inactive")

    def handle_ENDIF(self):
        if len(self.scope) == 1:
            raise Error("ENDIF must be preceded by IF")
        self.scope.pop()

    def handle_ELIF(self, key):
        if len(self.scope) == 1:
            raise Error("ELIF must be preceded by IF")
        elif self.scope[-1] == "active":
            self.scope[-1] = "done"
        elif self.scope[-1] == "inactive" and self._expand(key):
            self.scope[-1] = "active"

    def handle_ELSE(self):
        if len(self.scope) == 1:
            raise Error("ELSE must be preceded by IF")
        elif self.scope[-1] == "active":
            self.scope[-1] = "done"
        elif self.scope[-1] == "inactive":
            self.scope[-1] = "active"

    def handle_SET(self, text):
        parts = text.split("=", 1)
        if len(parts) == 1 or not parts[0]:
            raise Error('"SET %s": invalid format for SET. '
                        'Correct format is "SET identifier=value".' % text)
        self.expander.assign_value_(parts[0], parts[1])

    def push_alignment(self):
        self.alignments.append(self._alignment())

    def pop_alignment(self):
        self.alignments.pop()

    def align(self):
        self.cur_line.append(self.alignments[-1][self.column:])

    def _add_lines(self, lines):
        if not lines:
            return
        self.cur_line.append(lines[0])
        if len(lines) > 1:
            align = self._alignment()
            self.lines.append("".join(self.cur_line))
            self.cur_line = []
            self.column = 0
            for line in lines[1:-1]:
                if line:
                    self.lines.append(align + line)
                else:
                    self.lines.append("")
            self._add_text(align + lines[-1])

    def _add_text(self, text):
        npos = text.rfind("\n")
        if npos == -1:
            self.cur_line.append(text)
            self.column += len(text)
        else:
            self.cur_line.append(text[:npos])
            self.lines.append("".join(self.cur_line))
            self.cur_line = [text[npos + 1:]]
            self.column = len(text) - npos + 1

    @staticmethod
    def _split_key(text):
        b = text.find("(")
        if b == -1:
            return text, ()
        e = text.find(")", b + 1)
        if e == -1:
            raise Error("No closing parenthesis: " + text)
        return text[:b], [s.strip() for s in text[b + 1:e].split(",")]

    def _expand(self, key):
        key, params = self._split_key(key)
        return self.expander(key, params, (self._indentation(), self.column))

    def _indentation(self):
        for i, s in enumerate(self.cur_line):
            for j, c in enumerate(s):
                if c not in "\t ":
                    return "".join(self.cur_line[:i] + [self.cur_line[i][:j]])
        return ""

    def _alignment(self):
        ind = self._indentation()
        return ind + " " * (self.column - len(ind))


def make_lines(template, expander_func):
    proc = TemplateProcessor(template, expander_func)
    proc()
    return proc.lines


def make_text(template, expander_func):
    return "\n".join(make_lines(template, expander_func))


class LineBuilder:
    def __init__(self, line_width=80):
        self.line_width = line_width
        self.lines = []
        self.cur_line = []
        self.cur_line_width = 0

    def append(self, text, sep=" ", newline_sep="", first_sep=""):
        if self.cur_line_width == 0:
            self._add_text(first_sep, text)
        elif self.cur_line_width + len(text) + len(sep) <= self.line_width:
            self._add_text(sep, text)
        elif self.cur_line_width + len(newline_sep) <= self.line_width:
            self.cur_line.append(newline_sep)
            self.newline()
            self._add_text(text)
        elif len(self.cur_line) == 1:
            self.cur_line.append(newline_sep)
            self.newline()
            self._add_text(text)
        else:
            last = self.cur_line[-1]
            del self.cur_line[-2:]
            self.cur_line.append(newline_sep)
            self.newline()
            self._add_text(last)
            self.append(text, sep, newline_sep)

    def join(self, strings, sep=" ", newline_sep="", first_sep="", first_newline_sep=""):
        try:
            i = iter(strings)
            self.append(next(i), first_sep, first_newline_sep)
            while True:
                self.append(next(i), sep, newline_sep)
        except StopIteration:
            pass

    def newline(self):
        self.lines.append("".join(self.cur_line))
        self.cur_line = []
        self.cur_line_width = 0

    def build(self):
        if self.cur_line:
            self.newline()
        return self.lines

    def _add_text(self, *strings):
        self.cur_line.extend(s for s in strings if s)
        self.cur_line_width += sum(len(s) for s in strings)


def join(strings, line_width=80, sep=" ", newline_sep="", first_sep="", first_newline_sep=""):
    lb = LineBuilder(line_width)
    lb.join(strings, sep, newline_sep, first_sep, first_newline_sep)
    return lb.build()


def _translate_to_space(s, chars):
    # Slow (but not noticably so in this context) replacement for maketrans and
    # translate. I couldn't find a way to use maketrans and translate that
    # worked under both python2 and python3
    if chars not in _translate_to_space.transforms:
        _translate_to_space.transforms[chars] = set(c for c in chars)
    transform = _translate_to_space.transforms[chars]
    return "".join(" " if c in transform else c for c in s)


_translate_to_space.transforms = {}


_BadCppChars = "'!\"#$%&/()=?+*@.:,;<>^`-_[]{}"


def make_macro_name(s):
    return "_".join(_translate_to_space(s.upper(), _BadCppChars).split())


def variable_name(s):
    return "_".join(_translate_to_space(s, _BadCppChars).split())


if __name__ == "__main__":
    import sys

    def main(args):
        test_string = """\
std::unique_ptr<CommandLine> parseArgs(int argc, char* argv[])
{
    auto cmdLine = std::make_unique<CommandLine>();
    initialize(cmdLine->argParser);
    cmdLine->parseArgs(cmdLine->parsedArgs, argc, argv);

    if (!cmdLine->parsedArgs)
    {
        cmdLine->argParser.writeErrors(cmdLine->parsedArgs);
        return cmdLine;
    }
    if ([[[readHelpOpts]]])
    {
        cmdLine.writeHelp();
        cmdLine->result = CommandLine::Help;
        return cmdLine;
    }
[[[IF hasInfoOpts]]]
    if ([[[readInfoOpts]]])
    {
        cmdLine->result = CommandLine::Info;
        return cmdLine;
    }
[[[ENDIF]]]
    if ([[[incorrectArgCount]]])
    {
        cmdLine->argParser.writeMessage("Invalid number of arguments.");
        return cmdLine;
    }
[[[IF hasNormalOpts]]]
    [[[readNormalOpts]]]
[[[ENDIF]]]
[[[IF hasArgs]]]
    [[[readArgs]]]
[[[ENDIF]]]

    cmdLine->result = CommandLine::Ok;

    return cmdLine;
}"""
        print("\n".join(make_lines(test_string, DictExpander(
            {'hasInfoOpts': True,
             'readInfoOpts': ["kjell &&", "arne &&", "bjarne"],
             'readHelpOpts': "finn ||\n        tom"}))))
        return 0

    sys.exit(main(sys.argv[1:]))
