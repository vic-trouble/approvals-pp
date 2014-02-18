#pragma once

#include <list>
#include <map>
#include <fstream>
#include <process.h>
#include <set>
#include <string>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

namespace approvals
{

  template <class Object>
  void verify(const Object &object, const std::string &testcase);

  template <class Function, class ArgumentRange>
  void verify(const Function &function, const ArgumentRange &range, const std::string &testcase);

  template <class Function, class ArgumentRange1, class ArgumentRange2>
  void verify(const Function &function, const ArgumentRange1 &range1, const ArgumentRange2 &range2, const std::string &testcase);

  template <class Function, class ArgumentRange1, class ArgumentRange2, class ArgumentRange3>
  void verify(const Function &function, const ArgumentRange1 &range1, const ArgumentRange2 &range2, const ArgumentRange3 &range3, const std::string &testcase);

  template <class Object>
  std::string to_string(const Object &object);

  template <class Element>
  std::string to_string(const std::list<Element> &collection);

  template <class Element>
  std::string to_string(const std::vector<Element> &collection);

  template <class Element>
  std::string to_string(const std::set<Element> &collection);

  template <class Key, class Value>
  std::string to_string(const std::map<Key, Value> &collection);
}

namespace
{

  template <class Collection>
  std::string collection_to_string(const Collection &collection)
  {
    std::ostringstream stream;
    size_t index = 0;
    for (auto element: collection)
    {
      stream << index++ << " = " << element << std::endl;
    }
    return stream.str();
  }

  void verify_string(const std::string &received_text, const std::string &testcase)
  {
    std::string received_filename = testcase + ".received.txt";
    std::ofstream(received_filename) << received_text;

    std::string approved_filename = testcase + ".approved.txt";
    std::ostringstream approved;
    std::ifstream approved_file(approved_filename);

    bool test_pass = false, show_diff = true;
    if (approved_file.fail())
    {
      std::ofstream(approved_filename, std::ios_base::out); // try to create
      test_pass = true;
    }
    else
    {
      approved << approved_file.rdbuf();
      test_pass = received_text == approved.str();
      show_diff = !test_pass;
    }

    EXPECT_TRUE(test_pass);

    if (show_diff)
    {
#pragma warning(suppress: 4996) // unsafe function
      const char *raw_diff = getenv("LIB_APPROVALS_DIFF");
      if (!raw_diff || !*raw_diff)
        throw std::runtime_error("can't expand %LIB_APPROVALS_DIFF%");

      std::string diff(raw_diff);
      std::string command = "\"" + diff + "\"" + " " + received_filename + " " + approved_filename;

      if (system(command.c_str()) != 0)
        throw std::runtime_error("can't execute diff: " + command);
    }
  }
}

template <class Object>
void approvals::verify(const Object &object, const std::string &testcase)
{
  verify_string(to_string(object), testcase);
}

template <class Object>
std::string approvals::to_string(const Object &object)
{
  std::ostringstream stream;
  stream << object;
  return stream.str();
}

template <class Element>
std::string approvals::to_string(const std::list<Element> &collection)
{
  return ::collection_to_string(collection);
}

template <class Element>
std::string approvals::to_string(const std::vector<Element> &collection)
{
  return ::collection_to_string(collection);
}

template <class Element>
std::string approvals::to_string(const std::set<Element> &collection)
{
  return ::collection_to_string(collection);
}

template <class Key, class Value>
std::string approvals::to_string(const std::map<Key, Value> &collection)
{
  std::ostringstream stream;
  for (auto it = collection.cbegin(); it != collection.cend(); ++it)
  {
    stream << it->first << " = " << it->second << std::endl;
  }
  return stream.str();
}

template <class Function, class ArgumentRange>
void approvals::verify(const Function &function, const ArgumentRange &range, const std::string &testcase)
{
  std::ostringstream received;

  for (auto arg: range)
  {
    received << arg << " = ";
    try
    {
      received << function(arg);
    }
    catch (const std::exception &e)
    {
      received << e.what();
    }
    received << std::endl;
  }

  verify_string(received.str(), testcase);
}

template <class Function, class ArgumentRange1, class ArgumentRange2>
void approvals::verify(const Function &function, const ArgumentRange1 &range1, const ArgumentRange2 &range2, const std::string &testcase)
{
  std::ostringstream received;

  for (auto arg1: range1)
  {
    for (auto arg2: range2)
    {
      received << arg1 << ", " << arg2 << " = ";
      try
      {
        received << function(arg1, arg2);
      }
      catch (const std::exception &e)
      {
        received << e.what();
      }
      received << std::endl;
    }
  }

  verify_string(received.str(), testcase);
}

template <class Function, class ArgumentRange1, class ArgumentRange2, class ArgumentRange3>
void approvals::verify(const Function &function, const ArgumentRange1 &range1, const ArgumentRange2 &range2, const ArgumentRange3 &range3, const std::string &testcase)
{
  std::ostringstream received;

  for (auto arg1: range1)
  {
    for (auto arg2: range2)
    {
      for (auto arg3: range3)
      {
        received << arg1 << ", " << arg2 << ", " << arg3 << " = ";
        try
        {
          received << function(arg1, arg2, arg3);
        }
        catch (const std::exception &e)
        {
          received << e.what();
        }
        received << std::endl;
      }
    }
  }

  verify_string(received.str(), testcase);
}
