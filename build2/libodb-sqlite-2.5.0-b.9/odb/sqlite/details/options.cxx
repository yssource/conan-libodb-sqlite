// -*- C++ -*-
//
// This file was generated by CLI, a command line interface
// compiler for C++.
//

// Begin prologue.
//
//
// End prologue.

#include <odb/sqlite/details/options.hxx>

#include <map>
#include <set>
#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <cstring>
#include <fstream>

namespace odb
{
  namespace sqlite
  {
    namespace details
    {
      namespace cli
      {
        // unknown_option
        //
        unknown_option::
        ~unknown_option () throw ()
        {
        }

        void unknown_option::
        print (::std::ostream& os) const
        {
          os << "unknown option '" << option ().c_str () << "'";
        }

        const char* unknown_option::
        what () const throw ()
        {
          return "unknown option";
        }

        // unknown_argument
        //
        unknown_argument::
        ~unknown_argument () throw ()
        {
        }

        void unknown_argument::
        print (::std::ostream& os) const
        {
          os << "unknown argument '" << argument ().c_str () << "'";
        }

        const char* unknown_argument::
        what () const throw ()
        {
          return "unknown argument";
        }

        // missing_value
        //
        missing_value::
        ~missing_value () throw ()
        {
        }

        void missing_value::
        print (::std::ostream& os) const
        {
          os << "missing value for option '" << option ().c_str () << "'";
        }

        const char* missing_value::
        what () const throw ()
        {
          return "missing option value";
        }

        // invalid_value
        //
        invalid_value::
        ~invalid_value () throw ()
        {
        }

        void invalid_value::
        print (::std::ostream& os) const
        {
          os << "invalid value '" << value ().c_str () << "' for option '"
             << option ().c_str () << "'";

          if (!message ().empty ())
            os << ": " << message ().c_str ();
        }

        const char* invalid_value::
        what () const throw ()
        {
          return "invalid option value";
        }

        // eos_reached
        //
        void eos_reached::
        print (::std::ostream& os) const
        {
          os << what ();
        }

        const char* eos_reached::
        what () const throw ()
        {
          return "end of argument stream reached";
        }

        // file_io_failure
        //
        file_io_failure::
        ~file_io_failure () throw ()
        {
        }

        void file_io_failure::
        print (::std::ostream& os) const
        {
          os << "unable to open file '" << file ().c_str () << "' or read failure";
        }

        const char* file_io_failure::
        what () const throw ()
        {
          return "unable to open file or read failure";
        }

        // unmatched_quote
        //
        unmatched_quote::
        ~unmatched_quote () throw ()
        {
        }

        void unmatched_quote::
        print (::std::ostream& os) const
        {
          os << "unmatched quote in argument '" << argument ().c_str () << "'";
        }

        const char* unmatched_quote::
        what () const throw ()
        {
          return "unmatched quote";
        }

        // scanner
        //
        scanner::
        ~scanner ()
        {
        }

        // argv_scanner
        //
        bool argv_scanner::
        more ()
        {
          return i_ < argc_;
        }

        const char* argv_scanner::
        peek ()
        {
          if (i_ < argc_)
            return argv_[i_];
          else
            throw eos_reached ();
        }

        const char* argv_scanner::
        next ()
        {
          if (i_ < argc_)
          {
            const char* r (argv_[i_]);

            if (erase_)
            {
              for (int i (i_ + 1); i < argc_; ++i)
                argv_[i - 1] = argv_[i];

              --argc_;
              argv_[argc_] = 0;
            }
            else
              ++i_;

            return r;
          }
          else
            throw eos_reached ();
        }

        void argv_scanner::
        skip ()
        {
          if (i_ < argc_)
            ++i_;
          else
            throw eos_reached ();
        }

        // argv_file_scanner
        //
        bool argv_file_scanner::
        more ()
        {
          if (!args_.empty ())
            return true;

          while (base::more ())
          {
            // See if the next argument is the file option.
            //
            const char* a (base::peek ());
            const option_info* oi = 0;
            const char* ov = 0;

            if (!skip_)
            {
              if ((oi = find (a)) != 0)
              {
                base::next ();

                if (!base::more ())
                  throw missing_value (a);

                ov = base::next ();
              }
              else if (std::strncmp (a, "-", 1) == 0)
              {
                if ((ov = std::strchr (a, '=')) != 0)
                {
                  std::string o (a, 0, ov - a);
                  if ((oi = find (o.c_str ())) != 0)
                  {
                    base::next ();
                    ++ov;
                  }
                }
              }
            }

            if (oi != 0)
            {
              if (oi->search_func != 0)
              {
                std::string f (oi->search_func (ov, oi->arg));

                if (!f.empty ())
                  load (f);
              }
              else
                load (ov);

              if (!args_.empty ())
                return true;
            }
            else
            {
              if (!skip_)
                skip_ = (std::strcmp (a, "--") == 0);

              return true;
            }
          }

          return false;
        }

        const char* argv_file_scanner::
        peek ()
        {
          if (!more ())
            throw eos_reached ();

          return args_.empty () ? base::peek () : args_.front ().c_str ();
        }

        const char* argv_file_scanner::
        next ()
        {
          if (!more ())
            throw eos_reached ();

          if (args_.empty ())
            return base::next ();
          else
          {
            hold_[i_ == 0 ? ++i_ : --i_].swap (args_.front ());
            args_.pop_front ();
            return hold_[i_].c_str ();
          }
        }

        void argv_file_scanner::
        skip ()
        {
          if (!more ())
            throw eos_reached ();

          if (args_.empty ())
            return base::skip ();
          else
            args_.pop_front ();
        }

        const argv_file_scanner::option_info* argv_file_scanner::
        find (const char* a) const
        {
          for (std::size_t i (0); i < options_count_; ++i)
            if (std::strcmp (a, options_[i].option) == 0)
              return &options_[i];

          return 0;
        }

        void argv_file_scanner::
        load (const std::string& file)
        {
          using namespace std;

          ifstream is (file.c_str ());

          if (!is.is_open ())
            throw file_io_failure (file);

          while (!is.eof ())
          {
            string line;
            getline (is, line);

            if (is.fail () && !is.eof ())
              throw file_io_failure (file);

            string::size_type n (line.size ());

            // Trim the line from leading and trailing whitespaces.
            //
            if (n != 0)
            {
              const char* f (line.c_str ());
              const char* l (f + n);

              const char* of (f);
              while (f < l && (*f == ' ' || *f == '\t' || *f == '\r'))
                ++f;

              --l;

              const char* ol (l);
              while (l > f && (*l == ' ' || *l == '\t' || *l == '\r'))
                --l;

              if (f != of || l != ol)
                line = f <= l ? string (f, l - f + 1) : string ();
            }

            // Ignore empty lines, those that start with #.
            //
            if (line.empty () || line[0] == '#')
              continue;

            string::size_type p (line.find (' '));

            if (p == string::npos)
            {
              if (!skip_)
                skip_ = (line == "--");

              args_.push_back (line);
            }
            else
            {
              string s1 (line, 0, p);

              // Skip leading whitespaces in the argument.
              //
              n = line.size ();
              for (++p; p < n; ++p)
              {
                char c (line[p]);

                if (c != ' ' && c != '\t' && c != '\r')
                  break;
              }

              string s2 (line, p);

              // If the string is wrapped in quotes, remove them.
              //
              n = s2.size ();
              char cf (s2[0]), cl (s2[n - 1]);

              if (cf == '"' || cf == '\'' || cl == '"' || cl == '\'')
              {
                if (n == 1 || cf != cl)
                  throw unmatched_quote (s2);

                s2 = string (s2, 1, n - 2);
              }

              const option_info* oi;
              if (!skip_ && (oi = find (s1.c_str ())))
              {
                if (s2.empty ())
                  throw missing_value (oi->option);

                if (oi->search_func != 0)
                {
                  std::string f (oi->search_func (s2.c_str (), oi->arg));

                  if (!f.empty ())
                    load (f);
                }
                else
                  load (s2);
              }
              else
              {
                args_.push_back (s1);
                args_.push_back (s2);
              }
            }
          }
        }

        template <typename X>
        struct parser
        {
          static void
          parse (X& x, scanner& s)
          {
            using namespace std;

            const char* o (s.next ());

            if (s.more ())
            {
              string v (s.next ());
              istringstream is (v);
              if (!(is >> x && is.peek () == istringstream::traits_type::eof ()))
                throw invalid_value (o, v);
            }
            else
              throw missing_value (o);
          }
        };

        template <>
        struct parser<bool>
        {
          static void
          parse (bool& x, scanner& s)
          {
            s.next ();
            x = true;
          }
        };

        template <>
        struct parser<std::string>
        {
          static void
          parse (std::string& x, scanner& s)
          {
            const char* o (s.next ());

            if (s.more ())
              x = s.next ();
            else
              throw missing_value (o);
          }
        };

        template <typename X>
        struct parser<std::vector<X> >
        {
          static void
          parse (std::vector<X>& c, scanner& s)
          {
            X x;
            parser<X>::parse (x, s);
            c.push_back (x);
          }
        };

        template <typename X>
        struct parser<std::set<X> >
        {
          static void
          parse (std::set<X>& c, scanner& s)
          {
            X x;
            parser<X>::parse (x, s);
            c.insert (x);
          }
        };

        template <typename K, typename V>
        struct parser<std::map<K, V> >
        {
          static void
          parse (std::map<K, V>& m, scanner& s)
          {
            const char* o (s.next ());

            if (s.more ())
            {
              std::string ov (s.next ());
              std::string::size_type p = ov.find ('=');

              K k = K ();
              V v = V ();
              std::string kstr (ov, 0, p);
              std::string vstr (ov, (p != std::string::npos ? p + 1 : ov.size ()));

              int ac (2);
              char* av[] = 
              {
                const_cast<char*> (o), 0
              };

              if (!kstr.empty ())
              {
                av[1] = const_cast<char*> (kstr.c_str ());
                argv_scanner s (0, ac, av);
                parser<K>::parse (k, s);
              }

              if (!vstr.empty ())
              {
                av[1] = const_cast<char*> (vstr.c_str ());
                argv_scanner s (0, ac, av);
                parser<V>::parse (v, s);
              }

              m[k] = v;
            }
            else
              throw missing_value (o);
          }
        };

        template <typename X, typename T, T X::*M>
        void
        thunk (X& x, scanner& s)
        {
          parser<T>::parse (x.*M, s);
        }
      }
    }
  }
}

#include <map>
#include <cstring>

namespace odb
{
  namespace sqlite
  {
    namespace details
    {
      // options
      //

      options::
      options ()
      : database_ (),
        create_ (),
        read_only_ (),
        options_file_ ()
      {
      }

      options::
      options (int& argc,
               char** argv,
               bool erase,
               ::odb::sqlite::details::cli::unknown_mode opt,
               ::odb::sqlite::details::cli::unknown_mode arg)
      : database_ (),
        create_ (),
        read_only_ (),
        options_file_ ()
      {
        ::odb::sqlite::details::cli::argv_scanner s (argc, argv, erase);
        _parse (s, opt, arg);
      }

      options::
      options (int start,
               int& argc,
               char** argv,
               bool erase,
               ::odb::sqlite::details::cli::unknown_mode opt,
               ::odb::sqlite::details::cli::unknown_mode arg)
      : database_ (),
        create_ (),
        read_only_ (),
        options_file_ ()
      {
        ::odb::sqlite::details::cli::argv_scanner s (start, argc, argv, erase);
        _parse (s, opt, arg);
      }

      options::
      options (int& argc,
               char** argv,
               int& end,
               bool erase,
               ::odb::sqlite::details::cli::unknown_mode opt,
               ::odb::sqlite::details::cli::unknown_mode arg)
      : database_ (),
        create_ (),
        read_only_ (),
        options_file_ ()
      {
        ::odb::sqlite::details::cli::argv_scanner s (argc, argv, erase);
        _parse (s, opt, arg);
        end = s.end ();
      }

      options::
      options (int start,
               int& argc,
               char** argv,
               int& end,
               bool erase,
               ::odb::sqlite::details::cli::unknown_mode opt,
               ::odb::sqlite::details::cli::unknown_mode arg)
      : database_ (),
        create_ (),
        read_only_ (),
        options_file_ ()
      {
        ::odb::sqlite::details::cli::argv_scanner s (start, argc, argv, erase);
        _parse (s, opt, arg);
        end = s.end ();
      }

      options::
      options (::odb::sqlite::details::cli::scanner& s,
               ::odb::sqlite::details::cli::unknown_mode opt,
               ::odb::sqlite::details::cli::unknown_mode arg)
      : database_ (),
        create_ (),
        read_only_ (),
        options_file_ ()
      {
        _parse (s, opt, arg);
      }

      ::odb::sqlite::details::cli::usage_para options::
      print_usage (::std::ostream& os, ::odb::sqlite::details::cli::usage_para p)
      {
        CLI_POTENTIALLY_UNUSED (os);

        if (p != ::odb::sqlite::details::cli::usage_para::none)
          os << ::std::endl;

        os << "--database <filename> SQLite database file name. If the database file is not" << ::std::endl
           << "                      specified then a private, temporary on-disk database will" << ::std::endl
           << "                      be created. Use the :memory: special name to create a" << ::std::endl
           << "                      private, temporary in-memory database." << ::std::endl;

        os << std::endl
           << "--create              Create the SQLite database if it does not already exist." << ::std::endl
           << "                      By default opening the database fails if it does not" << ::std::endl
           << "                      already exist." << ::std::endl;

        os << std::endl
           << "--read-only           Open the SQLite database in read-only mode. By default" << ::std::endl
           << "                      the database is opened for reading and writing if" << ::std::endl
           << "                      possible, or reading only if the file is write-protected" << ::std::endl
           << "                      by the operating system." << ::std::endl;

        os << std::endl
           << "--options-file <file> Read additional options from <file>. Each option" << ::std::endl
           << "                      appearing on a separate line optionally followed by space" << ::std::endl
           << "                      and an option value. Empty lines and lines starting with" << ::std::endl
           << "                      # are ignored." << ::std::endl;

        p = ::odb::sqlite::details::cli::usage_para::option;

        return p;
      }

      typedef
      std::map<std::string, void (*) (options&, ::odb::sqlite::details::cli::scanner&)>
      _cli_options_map;

      static _cli_options_map _cli_options_map_;

      struct _cli_options_map_init
      {
        _cli_options_map_init ()
        {
          _cli_options_map_["--database"] = 
          &::odb::sqlite::details::cli::thunk< options, std::string, &options::database_ >;
          _cli_options_map_["--create"] = 
          &::odb::sqlite::details::cli::thunk< options, bool, &options::create_ >;
          _cli_options_map_["--read-only"] = 
          &::odb::sqlite::details::cli::thunk< options, bool, &options::read_only_ >;
          _cli_options_map_["--options-file"] = 
          &::odb::sqlite::details::cli::thunk< options, std::string, &options::options_file_ >;
        }
      };

      static _cli_options_map_init _cli_options_map_init_;

      bool options::
      _parse (const char* o, ::odb::sqlite::details::cli::scanner& s)
      {
        _cli_options_map::const_iterator i (_cli_options_map_.find (o));

        if (i != _cli_options_map_.end ())
        {
          (*(i->second)) (*this, s);
          return true;
        }

        return false;
      }

      bool options::
      _parse (::odb::sqlite::details::cli::scanner& s,
              ::odb::sqlite::details::cli::unknown_mode opt_mode,
              ::odb::sqlite::details::cli::unknown_mode arg_mode)
      {
        bool r = false;
        bool opt = true;

        while (s.more ())
        {
          const char* o = s.peek ();

          if (std::strcmp (o, "--") == 0)
          {
            opt = false;
            s.skip ();
            r = true;
            continue;
          }

          if (opt)
          {
            if (_parse (o, s))
            {
              r = true;
              continue;
            }

            if (std::strncmp (o, "-", 1) == 0 && o[1] != '\0')
            {
              // Handle combined option values.
              //
              std::string co;
              if (const char* v = std::strchr (o, '='))
              {
                co.assign (o, 0, v - o);
                ++v;

                int ac (2);
                char* av[] =
                {
                  const_cast<char*> (co.c_str ()),
                  const_cast<char*> (v)
                };

                ::odb::sqlite::details::cli::argv_scanner ns (0, ac, av);

                if (_parse (co.c_str (), ns))
                {
                  // Parsed the option but not its value?
                  //
                  if (ns.end () != 2)
                    throw ::odb::sqlite::details::cli::invalid_value (co, v);

                  s.next ();
                  r = true;
                  continue;
                }
                else
                {
                  // Set the unknown option and fall through.
                  //
                  o = co.c_str ();
                }
              }

              switch (opt_mode)
              {
                case ::odb::sqlite::details::cli::unknown_mode::skip:
                {
                  s.skip ();
                  r = true;
                  continue;
                }
                case ::odb::sqlite::details::cli::unknown_mode::stop:
                {
                  break;
                }
                case ::odb::sqlite::details::cli::unknown_mode::fail:
                {
                  throw ::odb::sqlite::details::cli::unknown_option (o);
                }
              }

              break;
            }
          }

          switch (arg_mode)
          {
            case ::odb::sqlite::details::cli::unknown_mode::skip:
            {
              s.skip ();
              r = true;
              continue;
            }
            case ::odb::sqlite::details::cli::unknown_mode::stop:
            {
              break;
            }
            case ::odb::sqlite::details::cli::unknown_mode::fail:
            {
              throw ::odb::sqlite::details::cli::unknown_argument (o);
            }
          }

          break;
        }

        return r;
      }
    }
  }
}

// Begin epilogue.
//
//
// End epilogue.

