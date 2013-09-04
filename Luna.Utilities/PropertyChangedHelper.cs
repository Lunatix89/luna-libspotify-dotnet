using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Linq.Expressions;
using System.Text;

namespace Luna.Utilities {
	public static class PropertyChangedHelper {

		public static void Raise<T>(this PropertyChangedEventHandler handler, Expression<Func<T>> propertyExpression) {
			if (handler != null) {
				var body = propertyExpression.Body as MemberExpression;
				var expression = body.Expression as ConstantExpression;
				handler(expression.Value, new PropertyChangedEventArgs(body.Member.Name));
			}
		}

	}
}
