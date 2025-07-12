library(readr)
library(ggplot2)
library(dplyr)

dataI <- read_delim("Resources/dati.csv",col_names = FALSE, escape_double = FALSE, trim_ws = TRUE)
speedups<- read_csv("saved/DatasetKaggle/speedup.csv",col_names = FALSE)
#speedups<- read_csv("saved/DatasetNew/speedup.csv",col_names = FALSE)

reps=length(speedups$X1)/9;
threads=c(rep(1,reps),rep(2,reps),rep(4,reps),rep(8,reps),rep(16,reps),rep(32,reps),rep(64,reps),rep(128,reps),rep(256,reps))
speedups$threads=threads
colnames(speedups)=c("Speedup","Threads")
speedups$Threads=as.integer(speedups$Threads)
speedups$color=factor(speedups$Threads)


data <- read_csv("cmake-build-debug/Outputs/output.csv", 
                   col_names = FALSE, col_types = cols(X3 = col_integer()))

data$X3=factor(data$X3)

q=ggplot(dataI, aes(x=X1,y=X2))+
  xlab("x")+
  ylab("y")+
  geom_point()
q
ggsave(filename = "Figures/Input.png", plot = q, width = 6, height = 4, dpi = 300)


p=ggplot(data,aes(x=X1,y=X2,color=X3))+
  geom_point()+
  guides(color = "none")+
  xlab("x")+
  ylab("y")+
  theme_grey()
p

sum(data$X3==data$X4)

ggsave(filename = "Figures/Cluster.png", plot = p, width = 6, height = 4, dpi = 300)


speed_agg=speedups %>%
  group_by(Threads) %>%
  summarise(Speedups = mean(Speedup), se = sd(Speedup), n = n())

r=ggplot(speedups,aes(x=Threads,y=Speedup),color=speedups$color)+
  geom_point()+
  xlab("Threads")+
  ylab("Speedup")+
  ggtitle("Speedups")+
  theme_grey()+
  xlim(0, 20)+
  stat_summary(fun = mean, geom = "point", color = "blue", size = 3) +
  stat_summary(fun.data = mean_cl_normal, geom = "errorbar", width = 0.2,color="blue")+
  geom_line(data=speed_agg, aes(x=Threads,y=Speedups),color="blue")+
  geom_abline(intercept = 0, slope = 1, color = "red", linetype = "dashed", linewidth = 1)+
  theme(plot.title = element_text(hjust = 0.5))
r
speed_agg$llim=speed_agg$Speedups-qt(0.025, speed_agg$n-1, lower.tail = FALSE)*speed_agg$se/sqrt(speed_agg$n)

speed_agg$ulim=speed_agg$Speedups+qt(0.025, speed_agg$n-1, lower.tail = FALSE)*speed_agg$se/sqrt(speed_agg$n)

ggsave(filename = "Figures/Speedup.png", plot = r, width = 6, height = 4, dpi = 300)

speed_agg
