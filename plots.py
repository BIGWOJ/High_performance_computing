import matplotlib.pyplot as plt

package_sizes = [10,25,50,150,250]
base_time = [43545] * len(package_sizes)

schedule_static_table = [4741,4836,5725,13724,17849]
schedule_static_collapse_table = [4527,4633,4980,4763,7152]
schedule_dynamic_table = [4656,4981,5390,13553,17816]
schedule_dynamic_collapse_table = [4585,4438,4601,4465,4471]

plt.figure(figsize=(18, 10))
plt.subplot(1,2,1)
plt.plot(package_sizes, base_time, color="green", label="Czas bazowy, bez #pragma")
plt.title("Klauzula schedule static")
plt.scatter(package_sizes, schedule_static_table, color="red", zorder=3)
plt.scatter(package_sizes, schedule_static_collapse_table, color="red", zorder=3)
plt.plot(package_sizes, schedule_static_table, label="Bez collapse(2)")
plt.plot(package_sizes, schedule_static_collapse_table, label="collapse(2)")
plt.xlabel("Rozmiar pakietów")
plt.ylabel("Czas [ms]")
plt.xticks(package_sizes)
plt.legend()

plt.subplot(1,2,2)
plt.title("Klauzula schedule dynamic")
plt.plot(package_sizes, base_time, color="green", label="Czas bazowy, bez #pragma")
plt.scatter(package_sizes, schedule_dynamic_table, color="red", zorder=3)
plt.scatter(package_sizes, schedule_dynamic_collapse_table, color="red", zorder=3)
plt.plot(package_sizes, schedule_dynamic_table, label="Bez collapse(2)")
plt.plot(package_sizes, schedule_dynamic_collapse_table, label="collapse(2)")
plt.xlabel("Rozmiar pakietów")
plt.ylabel("Czas [ms]")
plt.xticks(package_sizes)
plt.legend()

plt.show()
