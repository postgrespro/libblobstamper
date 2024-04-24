# NAME

libtap++ - C++ unit tests for the Test Anything Protocol

# SYNOPSIS

    #include <tap++/tap++.h>
    #include <string>

    using namespace TAP;

    int foo(int ronkle = 1) {
      return ronkle;
    }

    std::string bar() {
      return "a string";
    }

    int main() {
      plan(4);
      ok(true, "This test passes");
      is(foo(), 1, "foo() should be 1");
      is(bar(), "a string", "bar() should be \"a string\"");

      TODO="Foo is not completely implemented";
      is(foo(2), 5, "foo(2) should be 5");
      TODO="";

      return exit_status();
    }

# DESCRIPTION

`libtap++` is a TAP producer for C++ programs.

# HISTORY

libtap++ was originally developed as a part of
[libperl++](https://github.com/Leont/libperl--) project by Leon Timmermans

Then forked into separate [libtap++](https://github.com/cbab/libtappp)
project by Christian Babeux

Now libtap++ us supported by Nikolay Shaplov at
[https://gitlab.com/dhyannataraj/libtappp](https://gitlab.com/dhyannataraj/libtappp)

# FUNCTIONS

All functions and variables are defined in the `TAP` namespace.

## I love it when a plan comes together

Before anything else, you need a testing plan.  This basically declares
how many tests your script is going to run to protect against premature
failure.

- **plan()**

        void plan(int number_of_tests);
        void plan(skip_all, const std::string& reason="");
        void plan(no_plan);

    The function `plan` is used to indicate the plan of your test run. Usually you
    will just give it the number of tests as argument.

    Alternatively, you can give it the `skip_all` or `no_plan` constants as
    arguments. The first means you will not run the tests at all, the second means
    you will run an unknown number of tests (the latter is not recommended).

- **done\_testing()**

        void done_testing();
        void done_testing(int number_of_tests);

    If you don't know how many tests you're going to run, you can issue the plan
    when you're done running tests.

    number\_of\_tests is the same as plan(), it's the number of tests you expected to
    run. You can omit this, in which case the number of tests you ran doesn't
    matter, just the fact that your tests ran to conclusion.

    This is safer than and replaces the "no\_plan" plan.

## Test names

By convention, each test is assigned a number in order.  This is
largely done automatically for you.  However, it's often very useful to
assign a name to each test.  Which would you rather see:

    ok 4
    not ok 5
    ok 6

or

    ok 4 - basic multi-variable
    not ok 5 - simple exponential
    ok 6 - force == mass * acceleration

The later gives you some idea of what failed.  It also makes it easier
to find the test in your script, simply search for "simple
exponential".

All test functions take a name argument.  It's optional, but highly
suggested that you use it.

## I'm ok, you're not ok.

The basic purpose of this module is to print out either "ok #" or "not
ok #" depending on if a given test succeeded or failed.  Everything
else is just gravy.

All of the following print "ok" or "not ok" depending on if the test
succeeded or failed.  They all also return true or false,
respectively.

- **ok()**

        bool ok(bool condition, const std::string& test_name = "");

    `ok` is the basic test expression in TAP.  It simply evaluates any expression,
    for example, _got == expected_, taking a true value to mean that the test
    passed and a false value to mean that the test failed.

    `test_name` is a very short description of the test that will be printed out.
    It makes it very easy to find a test in your script when it fails and gives
    others an idea of your intentions. `test_name` is optional, but we very
    strongly encourage its use.

- **is()**
- **isnt()**

        template<typename T, typename U> bool is(const T& got, const U& expected, std::string& test_name = "");
        template<typename T, typename U> bool isnt(const T& got, const U& expected, std::string& test_name = "");

    Similar to ok(), is() and isnt() compare their two arguments
    with `==` and `!=` respectively and use the result of that to
    determine if the test succeeded or failed.  So these:

        # Is the ultimate answer 42?
        is( ultimate_answer(), 42, "Meaning of Life" );

        # foo isn't empty
        isnt( foo, "",     "Got some foo" );

    are similar to these:

        ok( ultimate_answer() == 42, "Meaning of Life" );
        ok( foo != "", "Got some foo" );

    (Mnemonic:  "This is that."  "This isn't that.")

    So why use these? They produce better diagnostics on failure. ok() cannot know
    what you are testing for (beyond the name), but is() and isnt() know what the
    test was and why it failed. For example this test:

        std::string foo("waffle"), bar("yarblokos");
        is( foo, bar, 'Is foo the same as bar?' );

    Will produce something like this:

        not ok 17 - Is foo the same as bar?
        #   Failed test 'Is foo the same as bar?'
        #          got: 'waffle'
        #     expected: 'yarblokos'

- **pass()**
- **fail()**

        bool pass(const std::string& test_name = "");
        bool fail(const std::string& test_name = "");

    Sometimes you just want to say that the tests have passed.  Usually
    the case is you've got some complicated condition that is difficult to
    wedge into an ok().  In this case, you can simply use pass() (to
    declare the test ok) or fail (for not ok).  They are synonyms for
    ok(true, test\_name) and ok(false, test\_name).

    Use these very, very, very sparingly.

## Conditional testing

- **skip()**

        void skip(int number, const std::string& reason = "");

    `skip` tells the TAP harness that you're skipping a _number_ of tests for the
    given _reason_.  Note that you have to do the skipping yourself.

- **TODO**

        {
        todo_guard why;
        TODO="why"
        my_tests_here ...
        }

    `TODO` is a global string variable that tells TAP harness the reason
    the current test is expected to fail.  You set TODO before a block of
    tests that you expect to fail and then unset it afterwards.  When TODO
    is the empty string, then the harness considers that there is no
    reason for the test to fail.  However, when TODO is non-empty, any
    failing test is not counted against the test suite and any succeeding
    test is reported as an unexpected success.

    The nice part about todo tests, as opposed to simply commenting out a
    block of tests, is it's like having a programmatic todo list. You know
    how much work is left to be done, you're aware of what bugs there are,
    and you'll know immediately when they're fixed.

    Note that TODO manipulates a global variable.  Thus, you should be
    careful to set it to "" before going to another section of the
    program.  An easy mistake to make is to have a failing section of code
    that throws an exception, taking you out of the current scope without
    resetting TODO. To make it easier to deal with this in a thread-safe
    manner, the todo\_guard class is provided. Objects of this class will
    reset TODO when they fall out of scope.

## Diagnostics

If you pick the right test function, you'll usually get a good idea of
what went wrong when it failed.  But sometimes it doesn't work out
that way.  So here we have ways for you to write your own diagnostic
messages which are safer than just `print STDERR`.

- **diag**

        diag(diagnostic_message...);

    Prints a diagnostic message which is guaranteed not to interfere with
    test output. The arguments are simply concatenated together.

    Returns false, so as to preserve failure.

    Handy for this sort of thing:

        ok( has_user("foo"), "There's a foo user" ) or
            diag("Since there's no foo, check that /etc/bar is set up right");

    which would produce:

        not ok 42 - There's a foo user
        #   Failed test 'There's a foo user'
        # Since there's no foo, check that /etc/bar is set up right.

    You might remember `ok() or diag()` with the mnemonic `open() or
    die()`.

    **NOTE** The exact formatting of the diagnostic output is still
    changing, but it is guaranteed that whatever you throw at it it won't
    interfere with the test.

- **note**

        note(diagnostic_message...);

    Like diag(), except the message will not be seen when the test is run
    in a harness.  It will only be visible in the verbose TAP stream.

    Handy for putting in notes which might be useful for debugging, but
    don't indicate a problem.

        note("Tempfile is ", tempfile);

    `diag` simply catenates its arguments to the error output, while `note`
    prints diagnostics to the TAP stream.

- **set\_output()**
- **set\_error()**

        void set_output(std::ofstream& new_output);
        void set_error(std::ofstream& new_error);

    These set the filehandle of the TAP stream and the error stream. They default
    to `std::cout` and `std::cerr`, respectively. These can only be set before any
    output is written to them.

## Ending a test run

- **exit\_status()**

    If all your tests passed, Test::Builder will exit with zero (which is
    normal).  If anything failed it will exit with how many failed.  If
    you run less (or more) tests than you planned, the missing (or extras)
    will be considered failures.  If the test died, even after having
    successfully completed all its tests, it will still be considered a failure
    and will exit with 255.

    So the exit codes are...

        0                   all tests successful
        255                 test died or all passed but wrong # of tests run
        any other number    how many failed (including missing or extras)

    If you fail more than 254 tests, it will be reported as 254.

- **bail\_out()**

        int exit_status();
        void bail_out(const std::string& reason);

    **bail\_out** terminates the current test program with exit code 255, indicating
    to the test harness that all subsequent testing should halt.  Typically this
    is used to indicate that testing cannot continue at all.

# SEE ALSO

[http://www.testanything.org](http://www.testanything.org)

[Test::More](https://metacpan.org/pod/Test%3A%3AMore) and [prove(1)](http://man.he.net/man1/prove) are the traditional perl client library and TAP
harness, respectively. This library is modeled after Test::More.

# AUTHORS

Leon Timmermans wrote `libtap++`. He stole much of this documentation from
Test::More. Mike Pomraning also contributed this documentation.

# COPYRIGHT

Copyright (c) 2008, 2009, 2010 Leon Timmermans.

See the LICENSE file for details.
