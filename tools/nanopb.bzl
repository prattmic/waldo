# FIXME(prattmic): This is from https://github.com/prattmic/nanopb 4dfed8c.
# Reference it directly once bazel is capable.

def _proto_pb(ctx):
    src = ctx.file.src
    out = ctx.outputs.out

    cmd = "protoc -o %s %s" % (out.path, src.path)

    ctx.action(
        inputs=[src],
        outputs=[out],
        command = cmd,
        progress_message = "Compiling %s to create %s" \
            % (src.basename, out.basename),
    )

proto_pb = rule(
    implementation=_proto_pb,
    attrs={
        "src": attr.label(
            mandatory=True,
            allow_files=True,
            single_file=True,
        ),
    },
    outputs={"out": "%{name}.pb"},
)

def _nanopb_srcs(ctx):
    proto = ctx.file.proto
    _renamed_proto = ctx.outputs._renamed_proto
    nanopb_generator = ctx.executable._nanopb_generator

    include_dir = ctx.outputs.source.short_path.rsplit("/", 1)[0]

    # nanopb_generator simply creates FOO.pb.h and FOO.pb.c from FOO.pb,
    # so we need to make sure proto actually looks like FOO.pb.
    ctx.action(
        inputs=[proto],
        outputs=[_renamed_proto],
        command = "cp %s %s" % (proto.path, _renamed_proto.path),
    )

    ctx.action(
        inputs=[_renamed_proto],
        outputs=[
            ctx.outputs.source,
            ctx.outputs.header,
        ],
        arguments = [
            "--generated-include-format",
            '#include "%s/%%s"' % (include_dir),
            "--library-include-format",
            '#include "%s"',
            _renamed_proto.path
        ],
        executable = nanopb_generator,
        progress_message = "Generating nanopb C source from %s" % (proto.basename),
    )

nanopb_srcs = rule(
    implementation=_nanopb_srcs,
    attrs={
        "_nanopb_generator": attr.label(
                executable=True,
                default=Label("@nanopb//generator:nanopb_generator"),
        ),
        "proto": attr.label(
            mandatory=True,
            allow_files=True,
            single_file=True,
        ),
    },
    outputs={
        "_renamed_proto": "%{name}.pb",
        "source": "%{name}.pb.c",
        "header": "%{name}.pb.h",
    },
    # Required when generating headers.
    output_to_genfiles=True,
)

def nanopb_library(name, proto):
    pb_name = name + "_pb"
    srcs_name = name + "_srcs"

    proto_pb(
        name = pb_name,
        src = proto,
    )

    nanopb_srcs(
        name = srcs_name,
        proto = pb_name,
    )

    native.cc_library(
        name = name,
        srcs = [srcs_name + ".pb.c"],
        hdrs = [srcs_name + ".pb.h"],
        deps = ["@nanopb//:nanopb"],
        # We use C++ features, even though these are .c
        copts = [
            "-x",
            "c++",
            "-std=c++0x",
        ],
    )

def _nanopb_cpp_srcs(ctx):
    proto = ctx.file.proto
    nanopb_cpp_generator = ctx.executable._nanopb_cpp_generator

    ctx.action(
        inputs=[proto],
        outputs=[
            ctx.outputs.out,
        ],
        arguments = [
            "--include",
            ctx.file.nanopb_include.short_path,
            "--output",
            ctx.outputs.out.path,
            proto.path,
        ],
        executable = nanopb_cpp_generator,
        progress_message = "Generating nanopb C++ source from %s" \
            % (proto.basename),
    )

nanopb_cpp_srcs = rule(
    implementation=_nanopb_cpp_srcs,
    attrs={
        "_nanopb_cpp_generator": attr.label(
                executable=True,
                default=Label("@nanopb//generator:nanopb_cpp_generator"),
        ),
        "proto": attr.label(
            mandatory=True,
            allow_files=True,
            single_file=True,
        ),
        "out": attr.output(mandatory=True),
        # Unfortunately Skylark does not expose the headers available from
        # a cc_library, so we just pass the header directly.
        "nanopb_include": attr.label(
            mandatory=True,
            allow_files=True,
            single_file=True,
        ),
    },
    # Required when generating headers.
    output_to_genfiles=True,
)

def nanopb_cpp_library(name, proto, lib=None):
    """Create a C++ nanopb library from a proto or nanopb_library.

    Args:
        name: Target name
        proto: Source proto.
        lib: Optional nanopb_library target. One will be created if not
             specified.
    """
    if not lib:
        lib = name + "_nanopb"
        nanopb_library(
            name = lib,
            proto = proto,
        )

    srcs_name = name + "_srcs"
    pb_name = name + "_pb"
    header_name = "%s.pb.hpp" % (name)

    proto_pb(
        name = pb_name,
        src = proto,
    )

    nanopb_cpp_srcs(
        name = srcs_name,
        proto = pb_name,
        out = header_name,
        # We have to depend on internal details of nanopb_library here.
        nanopb_include = lib + "_srcs.pb.h"
    )

    native.cc_library(
        name = name,
        hdrs = [header_name],
        deps = [
            lib,
            "@nanopb//util/task:status",
        ],
    )
