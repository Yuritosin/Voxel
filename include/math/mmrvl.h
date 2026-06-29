#ifndef INCLUDE_MMRVL_H
#define INCLUDE_MMRVL_H

namespace mmr {
	template <typename T>
	class mmrvl {
	public:
		mmrvl(const T& value): m_Value(value), m_PrevValue(value) {}

		void set(const T& value) {
			m_PrevValue = m_Value;
			m_Value = value;
		}

		void sync() { m_PrevValue = m_Value; }

		bool changed() const { return m_Value != m_PrevValue; }

		T now() const { return m_Value; }
		T prev() const { return m_PrevValue; }

		mmrvl<T>& operator=(const T& other) {
			set(other);
			return *this;
		}

		mmrvl<T> operator+(const T& other) const {
			return mmrvl<T>(m_Value + other);
		}
		mmrvl<T> operator-(const T& other) const {
			return mmrvl<T>(m_Value - other);
		}
		mmrvl<T> operator*(const T& other) const {
			return mmrvl<T>(m_Value * other);
		}
		mmrvl<T> operator/(const T& other) const {
			return mmrvl<T>(m_Value / other);
		}
		mmrvl<T> operator%(const T& other) const {
			return mmrvl<T>(m_Value % other);
		}

		mmrvl<T>& operator+=(const T& other) {
			set(m_Value + other);
			return *this;
		}
		mmrvl<T>& operator-=(const T& other) {
			set(m_Value - other);
			return *this;
		}
		mmrvl<T>& operator*=(const T& other) {
			set(m_Value * other);
			return *this;
		}
		mmrvl<T>& operator/=(const T& other) {
			set(m_Value / other);
			return *this;
		}
		mmrvl<T>& operator%=(const T& other) {
			set(m_Value % other);
			return *this;
		}

		bool operator==(const T& other) const {
			return m_Value == other;
		}
		bool operator!=(const T& other) const {
			return m_Value != other;
		}
		bool operator<(const T& other) const {
			return m_Value < other;
		}
		bool operator<=(const T& other) const {
			return m_Value <= other;
		}
		bool operator>(const T& other) const {
			return m_Value > other;
		}
		bool operator>=(const T& other) const {
			return m_Value >= other;
		}

		mmrvl<T> operator-() const {
			return mmrvl<T>(-m_Value);
		}
		mmrvl<T> operator+() const {
			return *this;
		}
	protected:
		T m_Value;
		T m_PrevValue;
	};
}

#endif
