#include <unity.h>

#include <Preferences.h>

void setUp(void) {
}

void tearDown(void) {
}


void test_bytes() {
#ifdef ESP32
  static const char* bytes[] = {
    "12", "AB", "Hello",
    "12345678901234567890123456789012345678901234567890"
  };
  static uint32_t sizes[] = { 2, 2, 5, 50 };
  const unsigned  testsCount = 4;
#else
  static const char* bytes[] = {
    "", "1", "12", "AB", "Hello",
    "12345678901234567890123456789012345678901234567890"
  };
  static uint32_t sizes[] = { 0, 1, 2, 2, 5, 50 };
  const unsigned  testsCount = 6;
#endif

  Preferences prefs;
  TEST_ASSERT_TRUE(prefs.begin("test"));

  for (int i = 0; i < testsCount; i++) {
    const char* data = bytes[i];
    uint32_t len  = sizes[i];
    uint8_t buf[128] = { 0, };


    TEST_ASSERT_EQUAL_UINT(len, prefs.putBytes("aaa", data, len));
    TEST_ASSERT_TRUE(prefs.isKey("aaa"));
    TEST_ASSERT_EQUAL_UINT(len, prefs.getBytesLength("aaa"));
    TEST_ASSERT_EQUAL_UINT(len, prefs.getBytes("aaa", buf, sizeof(buf)));
    if (len) {
      TEST_ASSERT_EQUAL_MEMORY(data, buf, len);
    }
  }

  // Do the same, but decreasing size
  for (int i = testsCount-2; i >= 0; i--) {
    const char* data = bytes[i];
    uint32_t len  = sizes[i];
    uint8_t buf[128] = { 0, };

    TEST_ASSERT_EQUAL_UINT(len, prefs.putBytes("aaa", data, len));
    TEST_ASSERT_TRUE(prefs.isKey("aaa"));
    TEST_ASSERT_EQUAL_UINT(len, prefs.getBytesLength("aaa"));
    TEST_ASSERT_EQUAL_UINT(len, prefs.getBytes("aaa", buf, sizeof(buf)));
    if (len) {
      TEST_ASSERT_EQUAL_MEMORY(data, buf, len);
    }
  }

  prefs.clear();
}

void test_remove_key() {
  Preferences prefs;
  TEST_ASSERT_TRUE(prefs.begin("test"));

  // Add values and verify
  TEST_ASSERT_EQUAL_UINT(7, prefs.putString("aaa", "value A"));
  TEST_ASSERT_EQUAL_UINT(7, prefs.putString("bbb", "value B"));
  TEST_ASSERT_TRUE(prefs.isKey("aaa"));
  TEST_ASSERT_TRUE(prefs.isKey("bbb"));
  TEST_ASSERT_EQUAL_STRING("value A", prefs.getString("aaa").c_str());
  TEST_ASSERT_EQUAL_STRING("value B", prefs.getString("bbb").c_str());

  // Remove A, check that B is intact
  TEST_ASSERT_TRUE(prefs.remove("aaa"));
  TEST_ASSERT_FALSE(prefs.isKey("aaa"));
  TEST_ASSERT_TRUE(prefs.isKey("bbb"));
  TEST_ASSERT_EQUAL_STRING("", prefs.getString("aaa").c_str());
  TEST_ASSERT_EQUAL_STRING("value B", prefs.getString("bbb").c_str());

  prefs.clear();
}

void test_clear_namespace() {
  Preferences prefsA, prefsB;
  TEST_ASSERT_TRUE(prefsA.begin("testA"));
  TEST_ASSERT_TRUE(prefsB.begin("testB"));

  // Add values and verify
  TEST_ASSERT_EQUAL_UINT(7, prefsA.putString("value", "value A"));
  TEST_ASSERT_EQUAL_UINT(7, prefsB.putString("value", "value B"));
  TEST_ASSERT_EQUAL_STRING("value A", prefsA.getString("value").c_str());
  TEST_ASSERT_EQUAL_STRING("value B", prefsB.getString("value").c_str());

  // Clear A, check that B is intact
  TEST_ASSERT_TRUE(prefsA.clear());
  TEST_ASSERT_EQUAL_STRING("", prefsA.getString("value").c_str());
  TEST_ASSERT_EQUAL_STRING("value B", prefsB.getString("value").c_str());

  // Clear B
  TEST_ASSERT_TRUE(prefsB.clear());
  TEST_ASSERT_EQUAL_STRING("", prefsB.getString("value").c_str());

  // Add values and verify (again)
  TEST_ASSERT_EQUAL_UINT(9, prefsA.putString("value", "value AAA"));
  TEST_ASSERT_EQUAL_UINT(9, prefsB.putString("value", "value BBB"));
  TEST_ASSERT_EQUAL_STRING("value AAA", prefsA.getString("value").c_str());
  TEST_ASSERT_EQUAL_STRING("value BBB", prefsB.getString("value").c_str());

  prefsA.clear();
  prefsB.clear();
}

void test_utf8_key() {
  Preferences prefs;
  TEST_ASSERT_TRUE(prefs.begin("test"));
  TEST_ASSERT_EQUAL_UINT(4, prefs.putUInt("😁", 1234));
  TEST_ASSERT_EQUAL_UINT(1234, prefs.getUInt("😁"));

  prefs.clear();
}

void test_utf8_value() {
  Preferences prefs;
  TEST_ASSERT_TRUE(prefs.begin("test"));
  TEST_ASSERT_EQUAL_UINT(4, prefs.putString("unicode", "😁"));
  TEST_ASSERT_EQUAL_STRING("😁", prefs.getString("unicode").c_str());

  prefs.clear();
}

int runUnityTests(void) {
  UNITY_BEGIN();

  RUN_TEST(test_bytes);
  RUN_TEST(test_utf8_key);
  RUN_TEST(test_utf8_value);
  RUN_TEST(test_remove_key);
  RUN_TEST(test_clear_namespace);

  return UNITY_END();
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println();

  runUnityTests();
}

void loop() {
  delay(100);
}
